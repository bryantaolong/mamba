# Roadmap

## 1.0.0 阻塞项

以下问题在 1.0.0 之前需要解决。

### Option 默认值

- **问题**：`AddOption` 不支持默认值，help 输出也不展示默认值
- **影响**：用户无法声明 `--output` 这类带默认值的选项，也无法在 help 中看到 `(default: ...)`
- **方向**：`AddOption` 增加默认值参数，`GetOption` 保持当前行为或提供 `HasOption` 区分"未设置"

### Action 退出码

- **问题**：action 签名是 `std::function<void(const ParsedArgs&)>`，无法返回 `int`
- **影响**：CLI 无法通过返回值表达成功/失败状态（如 `return 1;`）
- **方向**：考虑将 action 签名改为返回 `int`，或提供 `SetExitCode` 机制

### GetOption 空值语义

- **问题**：`GetOption` 用空字符串 `""` 表示"未设置"，但 option 值本身也可能是空字符串
- **影响**：无法区分 `--msg ""`（传了空值）和没传 `--msg`
- **方向**：提供 `HasOption(key)` 方法，或返回 `std::optional<std::string>`

## 后续版本

- 子命令嵌套（如 `pdfx tools foo`）
- Positional 参数声明（数量、类型、required）
- 自动补全脚本生成
- 配置文件和环境变量支持
- 更好的错误恢复（TryExecute / 优雅降级）
