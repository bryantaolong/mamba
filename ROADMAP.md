# Roadmap

## 1.0.0 阻塞项

以下问题在 1.0.0 之前需要解决。

### 必填校验无法识别短选项别名

* **状态**：已修复

* **表现**：`MarkAsRequired("--message")` 后，用户传 `-m msg` 仍被报 missing required option。

* **原因**：`Command::Execute` 只按 `required_` 原始文本匹配，不识别短别名；同时解析阶段会把紧跟选项名的已知选项误当成值，导致空值/漏值绕过。

* **修复**：

  * `src/command.cpp`：解析时统一将 flag/option 的 key 规范化为 `long_name` 存储，`GetOption`/`HasFlag` 按规范名查询即可，与用户敲长名还是短名无关；

  * `src/command.cpp`：required 校验先将必填名规范化为其 `long_name` 再查 `parsed`，与登记/敲法无关；

  * `src/command.cpp`：解析时若下一个 token 已在 `options_` 中，报 requires a value；

  * `include/mamba/command.h`：新增 `ParsedArgs::HasOption(key)`，方便业务区分“未设置”与“设置为空”。

### Option 默认值

* **状态**：已修复

* **问题**：`AddOption` 不支持默认值，help 输出也不展示默认值

* **影响**：用户无法声明 `--output` 这类带默认值的选项，也无法在 help 中看到 `(default: ...)`

* **修复**：

  * `include/mamba/command.h`：`OptionDef` 新增 `default_val` 字段；`AddOption` 新增带默认值的重载；

  * `src/command.cpp`：`Execute` 将声明的默认值带入 `ParsedArgs::defaults_`（不写入 `options_`，`HasOption` 仍可区分"未设置"与"设置为默认值"）；`GetOption` 按 显式值 → 声明默认值 → 调用方兜底的顺序回退；

  * `src/command.cpp`：`PrintHelp` 对带默认值的选项追加 `(default: ...)` 展示。

### Action 退出码

* **状态**：已修复

* **问题**：action 签名是 `std::function<void(const ParsedArgs&)>`，无法返回 `int`

* **影响**：CLI 无法通过返回值表达成功/失败状态（如 `return 1;`）

* **修复**：

  * `include/mamba/command.h` / `src/command.cpp`：action 签名改为 `std::function<int(const ParsedArgs&)>`；`Command::Execute` 返回 `int`，解析错误（缺值、缺必填项）返回非 0，成功时透传 action 的返回值；

  * `include/mamba/mamba.h` / `src/mamba.cpp`：`Mamba::Execute` 与 `Mamba::Run` 返回 `int` 并逐层透传，`main` 以 `return mamba.Run(argc, argv);` 结束进程。

### GetOption 空值语义

* **状态**：已修复

* **问题**：`GetOption` 用空字符串 `""` 表示"未设置"，但 option 值本身也可能是空字符串

* **影响**：无法区分 `--msg ""`（传了空值）和没传 `--msg`

* **修复**：`GetOption` 返回 `std::optional<std::string>`，默认值为 `std::nullopt`；调用方可通过 `HasOption` / optional 真值显式区分“未设置”和“值为空”。

## 后续版本

* 子命令嵌套（如 `pdfx tools foo`）

* Positional 参数声明（数量、类型、required）

* 自动补全脚本生成

* 配置文件和环境变量支持

* 更好的错误恢复（TryExecute / 优雅降级）

