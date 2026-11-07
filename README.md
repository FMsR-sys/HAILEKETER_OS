# HAILEKETER_OS
使用 NASM 汇编与 C++ 开发的、基于 x86‑32 架构的自研操作系统。
![license](https://img.shields.io/badge/license-AGPL--3.0-blue)
![arch](https://img.shields.io/badge/arch-x86--32-lightgrey)
![lang](https://img.shields.io/badge/C%2B%2B%20%2F%20NASM-%2F-yellowgreen)
## ✨ 已实现功能
### Bootloader
- A20 开启、GDT 加载
- E820 内存探测
- 磁盘读取（int 13h）
- 跳转 C++ 内核（实模式 → 保护模式）
### C++ 内核基础
- BSS 段清零
- 段布局紧凑化：`.text` / `.rodata` / `.data` / `.bss`，丢弃 `.eh_frame`
### VGA 文本终端
- 清屏、自定义文字颜色
- 单字符打印、字符串输出
- 自动换行、满屏画面向上滚动一行
- 输出 16 进制字符
- 硬件光标控制，边界保护
- 退格删除字符，跨行退格支持
- **512 行后台缓冲区，支持 PageUp / PageDown 翻阅历史日志**
- 按行局部刷新，键盘中断内不触发全屏重绘
### 内存分页
- 构建页目录与按需页表（不再一次性铺满 4GB）
- 恒等映射：虚拟地址 = 物理地址
- 映射范围按需确定：低端 1MB、内核镜像、栈区、可用物理内存
- 成功开启 CR0 分页位，流水线刷新正常
### 中断基础框架
- 端口 IO 封装重制
- PIC 8259 中断重映射、EOI 发送
- IDT 中断描述符表、ISR 中断服务例程完整框架
- 页错误（INT 0x0E）处理入口
### PMM 位图物理内存管理器
- 物理页分配与释放
- 读取 E820 内存地图标记可用空闲内存
- 自动标记内核占用内存区域，防止内核被覆盖
### 内核堆分配器
- `kmalloc` / `kfree` 动态内存接口
- 双向空闲块合并，减少内存碎片
- 基础重复释放检测
### VMM 虚拟内存管理器
- 复用 Bootloader 阶段建立的页目录
- 软件标记 bit9 作为内存堆标记
- 错误码体系完成
- 基础映射和释放
- 物理地址转换函数
- 切换页目录、刷新 TLB 缓存
- 缺页按需分配物理页并建立映射
### PS/2 键盘驱动 & 交互式控制台
- IRQ-1 键盘中断，扫描码读取解析
- QWERTY 字母输入、左右 Shift 大小写切换
- 回车键换行、退格删除
- 0xE0 扩展扫描码支持：方向键 ↑ ↓ ← →
- PageUp / PageDown 翻阅终端历史输出
- 按键防抖，屏蔽长按刷屏
- 扫描码 → ASCII 映射采用 `switch` 直查，避免数组错位
## 📝 开发计划
- [ ] 交互式内核 Shell，命令解析与内置指令
- [ ] CPU 异常处理、完善中断体系
- [ ] PAE 扩展
- [ ] 进程与任务调度
- [ ] 用户态特权级支持
- [ ] FAT32 文件系统，支持从磁盘加载程序
- [ ] 调试工具、内存越界保护检测
## ⚒️ 编译与运行
**环境要求**
| 项目 | 说明 |
| --- | --- |
| 交叉工具链 | `i686-elf-g++`、`i686-elf-ld`、`i686-elf-objcopy` |
| 汇编器 | NASM |
| 模拟器 | `qemu-system-i386` |
| 开发环境 | VSCode + MSYS2 |
**构建**
```bash
bash build.sh
```
`build.sh` 会依次完成：编译汇编与 C++ 源码 → 链接内核 → 生成 `kernel.bin` → 拼装可引导软盘镜像 `boot.img` → 用 QEMU 启动。
## 📂 项目结构
```
HAILEKETER_OS/
├─ boot1.asm / boot2.asm     # 两段式引导
├─ linker.ld                 # 内核链接脚本
├─ build.sh                  # 一键构建 + 启动
├─ kernel.cpp                # 内核主入口与初始化流程
├─ vga.cpp/.hpp              # VGA 文本终端
├─ e820.cpp/.hpp             # BIOS 内存地图读取
├─ paging.cpp/.hpp           # 分页初始化与恒等映射
├─ pmm.cpp/.hpp              # 物理内存管理器（位图）
├─ vmm.cpp/.hpp              # 虚拟内存管理器
├─ kmalloc.cpp/.hpp          # 内核堆
├─ idt.cpp/.hpp, idt.asm     # 中断描述符表
├─ isr.cpp/.hpp              # 中断服务例程 / 键盘驱动
├─ pic.cpp/.hpp              # 8259 中断控制器
├─ io.cpp/.hpp               # 端口 IO 封装
└─ shell.cpp/.hpp            # 控制台输入
```
## 📄 License
[AGPL-3.0](LICENSE)
