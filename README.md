# CheckVM DLL 调用 Wiki

> 虚拟机检测 DLL — 支持 x64 / x86，跨语言调用

支持检测以下虚拟机特征
vmware vbox kvm  qemu pve

支持c++,c#,Python,易语言,Delphi,VBA语言调用

官网：https://vmquxunihua.com  QQ：137879654

PanChat Online: https://pan.jiasu7.top/u/1

GUI DEMO 
<img width="818" height="826" alt="image" src="https://github.com/user-attachments/assets/921fb61a-fc00-45c2-b003-8ae2efc00421" />

---

## 一、文件清单

| 文件 | 说明 |
|---|---|
| `checkvm-x64.dll` | 64 位 DLL（运行时需要） |
| `checkvm-x86.dll` | 32 位 DLL |

**架构匹配规则：**
- 64 位程序（x64）→ 用 `checkvm-x64.dll`
- 32 位程序（x86）→ 用 `checkvm-x86.dll`
- **不能混用**，否则加载失败

---

## 二、导出函数（3 个）

### 函数 1：IsVirtualMachine（推荐，最简单）

```c
int IsVirtualMachine(void);
```

| 返回值 | 含义 |
|---|---|
| **1** | ✅ 检测为虚拟机 |
| **0** | ❌ 物理机 |
| **-1** | ⚠️ 检测失败（异常/初始化错误） |

### 函数 2：DetectVmEx（详细版）

```c
int DetectVmEx(
    int* isVm,       // [输出] 0=物理机, 1=虚拟机
    int* cond1,      // [输出] 条件1 (0/1)
    int* cond2,      // [输出] 条件2 (0/1)
    int* cond3,      // [输出] 条件3 (0/1)
    int* cond4,      // [输出] 条件4 (0/1)
    int* cond5,      // [输出] 条件5 (0/1)
    char* jsonBuf,   // [输出] JSON字符串缓冲区（可传 NULL）
    int jsonBufSize  // jsonBuf 大小（jsonBuf=NULL 时忽略）
);
```

| 返回值 | 含义 |
|---|---|
| **0** | ✅ 检测成功 |
| **-1** | ⚠️ 检测失败 |

**条件说明：**
| 参数 | 含义 |
|---|---|
| cond1 | 检测条件 1，为 1 时表示命中虚拟机特征 |
| cond2 | 检测条件 2，为 1 时表示命中虚拟机特征 |
| cond3 | 检测条件 3，为 1 时表示命中虚拟机特征 |
| cond4 | 检测条件 4，为 1 时表示命中虚拟机特征 |
| cond5 | 检测条件 5，为 1 时表示命中虚拟机特征 |

**判断规则：** 5 个条件中任意 2 个以上成立 → `isVm = 1`

**JSON 示例：**
```json
{"key1":"value1","key2":"value2","flag1":1,"flag2":true,"flag3":false}
```

### 函数 3：DetectVm（C++ 专用）

```cpp
VmDetectionSummary DetectVm();
```

返回完整结构体，仅 C++ 可用（其他语言调用结构体困难，建议用 DetectVmEx）。

---

## 三、调用示例

### C++（用导入库，隐式链接）

```cpp
#include <stdio.h>
#include "checkvm.h"

#pragma comment(lib, "checkvm.lib")    // 链接时自动找 checkvm-x64.dll

int main() {
    // 方式 1：简单版
    int isVm = IsVirtualMachine();
    if (isVm == 1)      printf("虚拟机\n");
    else if (isVm == 0) printf("物理机\n");
    else                printf("检测失败\n");

    // 方式 2：详细版
    int isVm2, c1, c2, c3, c4, c5;
    char json[512] = {0};
    int ret = DetectVmEx(&isVm2, &c1, &c2, &c3, &c4, &c5, json, sizeof(json));
    if (ret == 0) {
        printf("结果: %s\n", isVm2 ? "虚拟机" : "物理机");
        printf("条件: C1=%d C2=%d C3=%d C4=%d C5=%d\n", c1, c2, c3, c4, c5);
        printf("JSON: %s\n", json);
    }
    return 0;
}
```

**编译命令（MSVC）：**
```
cl /EHsc main.cpp checkvm.lib
```

---

### C++（动态加载，无 .lib 也能用）

```cpp
#include <windows.h>
#include <stdio.h>

typedef int (*FN_IsVM)();
typedef int (*FN_DetectEx)(int*,int*,int*,int*,int*,int*,char*,int);

int main() {
    HMODULE hDll = LoadLibraryA("checkvm-x64.dll");   // 或 "checkvm-x86.dll"
    if (!hDll) { printf("DLL加载失败\n"); return 1; }

    FN_IsVM IsVM = (FN_IsVM)GetProcAddress(hDll, "IsVirtualMachine");
    FN_DetectEx DetectEx = (FN_DetectEx)GetProcAddress(hDll, "DetectVmEx");

    if (IsVM) printf("IsVM: %d\n", IsVM());

    if (DetectEx) {
        int isVm, c1, c2, c3, c4, c5;
        char json[512] = {0};
        DetectEx(&isVm, &c1, &c2, &c3, &c4, &c5, json, sizeof(json));
        printf("结果: %s, JSON: %s\n", isVm ? "虚拟机" : "物理机", json);
    }

    FreeLibrary(hDll);
    return 0;
}
```

---

### C#（.NET / Unity / ABP）

```csharp
using System;
using System.Runtime.InteropServices;
using System.Text;

class CheckVm {
    // ===== 简单版 =====
    [DllImport("checkvm-x64.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int IsVirtualMachine();

    // ===== 详细版 =====
    [DllImport("checkvm-x64.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int DetectVmEx(
        out int isVm,
        out int cond1, out int cond2, out int cond3,
        out int cond4, out int cond5,
        StringBuilder jsonBuf, int jsonBufSize
    );

    static void Main() {
        // 简单调用
        int result = IsVirtualMachine();
        Console.WriteLine(result == 1 ? "虚拟机" : result == 0 ? "物理机" : "检测失败");

        // 详细调用
        int isVm, c1, c2, c3, c4, c5;
        StringBuilder json = new StringBuilder(512);
        DetectVmEx(out isVm, out c1, out c2, out c3, out c4, out c5, json, json.Capacity);
        Console.WriteLine($"结果: {(isVm == 1 ? "虚拟机" : "物理机")}");
        Console.WriteLine($"条件: C1={c1} C2={c2} C3={c3} C4={c4} C5={c5}");
        Console.WriteLine($"JSON: {json}");
    }
}
```

**注意事项：**
- 32 位 C# 程序 → 改用 `checkvm-x86.dll`
- `CallingConvention = CallingConvention.Cdecl` 必须加，否则栈不平衡会崩溃
- `jsonBuf` 必须用 `StringBuilder`，不能用 `string`（后者是只读）
- Unity 把 DLL 放到 `Assets/Plugins/x86_64/` 下，自动加载

---

### Python

```python
import ctypes
from ctypes import c_int, c_char_p, create_string_buffer

# 加载 DLL (64位用 checkvm-x64.dll, 32位用 checkvm-x86.dll)
dll = ctypes.CDLL("checkvm-x64.dll")

# ===== 简单版 =====
dll.IsVirtualMachine.restype = c_int
result = dll.IsVirtualMachine()
if result == 1:
    print("虚拟机")
elif result == 0:
    print("物理机")
else:
    print("检测失败")

# ===== 详细版 =====
dll.DetectVmEx.restype = c_int
dll.DetectVmEx.argtypes = [
    ctypes.POINTER(c_int),  # isVm
    ctypes.POINTER(c_int),  # cond1
    ctypes.POINTER(c_int),  # cond2
    ctypes.POINTER(c_int),  # cond3
    ctypes.POINTER(c_int),  # cond4
    ctypes.POINTER(c_int),  # cond5
    c_char_p,               # jsonBuf
    c_int,                  # jsonBufSize
]

isVm = c_int()
c1, c2, c3, c4, c5 = c_int(), c_int(), c_int(), c_int(), c_int()
json_buf = create_string_buffer(512)

ret = dll.DetectVmEx(
    ctypes.byref(isVm), ctypes.byref(c1), ctypes.byref(c2),
    ctypes.byref(c3),  ctypes.byref(c4), ctypes.byref(c5),
    json_buf, 512
)

print(f"结果: {'虚拟机' if isVm.value else '物理机'}")
print(f"条件: C1={c1.value} C2={c2.value} C3={c3.value} C4={c4.value} C5={c5.value}")
print(f"JSON: {json_buf.value.decode('utf-8')}")
```

---

### 易语言

```
.版本 2

.DLL命令 IsVirtualMachine, 整数型, "checkvm-x64.dll", "IsVirtualMachine"
    ' 返回: 1=虚拟机 0=物理机 -1=失败

.DLL命令 DetectVmEx, 整数型, "checkvm-x64.dll", "DetectVmEx"
    .参数 isVm, 整数型, 传址
    .参数 cond1, 整数型, 传址
    .参数 cond2, 整数型, 传址
    .参数 cond3, 整数型, 传址
    .参数 cond4, 整数型, 传址
    .参数 cond5, 整数型, 传址
    .参数 jsonBuf, 文本型, 传址
    .参数 jsonBufSize, 整数型

.子程序 检测演示
.局部变量 isVm, 整数型
.局部变量 json, 文本型
.局部变量 ret, 整数型

ret ＝ IsVirtualMachine ()
.如果真 (ret ＝ 1)
    信息框 ("检测为虚拟机", 0, , )
.如果真结束

json ＝ 取空白文本 (512)
ret ＝ DetectVmEx (isVm, c1, c2, c3, c4, c5, json, 512)
信息框 ("结果: " ＋ 到文本 (isVm), 0, , )
```

**易语言注意：**
- 易语言是 32 位，必须用 **checkvm-x86.dll**
- 所有 `整数型` 参数必须加 **"传址"** 修饰
- `jsonBuf` 传入前要先 `取空白文本(512)` 预分配

---

### Delphi / Lazarus

```pascal
unit CheckVm;

interface
  function IsVirtualMachine: Integer; stdcall; external 'checkvm-x64.dll';
  function DetectVmEx(var isVm, cond1, cond2, cond3, cond4, cond5: Integer;
                      jsonBuf: PChar; jsonBufSize: Integer): Integer; stdcall; external 'checkvm-x64.dll';

implementation

// 调用示例:
// var
//   isVm: Integer;
//   json: array[0..511] of Char;
// begin
//   IsVirtualMachine;
//   DetectVmEx(isVm, c1, c2, c3, c4, c5, @json[0], 512);
// end;
```

---

### VB6 / VBA

```vb
Private Declare Function IsVirtualMachine Lib "checkvm-x64.dll" () As Long
Private Declare Function DetectVmEx Lib "checkvm-x64.dll" _
    (ByRef isVm As Long, ByRef cond1 As Long, ByRef cond2 As Long, _
     ByRef cond3 As Long, ByRef cond4 As Long, ByRef cond5 As Long, _
     ByVal jsonBuf As String, ByVal jsonBufSize As Long) As Long

Sub Demo()
    Dim r As Long
    r = IsVirtualMachine()
    If r = 1 Then MsgBox "虚拟机" Else MsgBox "物理机"
    
    Dim isVm As Long, json As String
    json = String$(512, 0)
    DetectVmEx isVm, 0, 0, 0, 0, 0, json, 512
End Sub
```

---

## 四、常见问题 FAQ

### Q1: 调用后程序崩溃（Access Violation）

**原因：** 调用约定不匹配
- 本 DLL 用 **cdecl**（C 语言默认）
- Delphi/VB 默认是 **stdcall**
- C# 必须显式写 `CallingConvention = CallingConvention.Cdecl`

**解决：** 确保所有语言都用 `Cdecl` 调用约定。

---

### Q2: `LoadLibrary("checkvm-x64.dll")` 返回 NULL

| GetLastError | 含义 | 解决 |
|---|---|---|
| 998 (ERROR_NOACCESS) | 位数不匹配 | 64 位程序加载了 x86 DLL，换用 `checkvm-x86.dll` |
| 126 (ERROR_MOD_NOT_FOUND) | DLL 找不到 | 放到 EXE 同目录，或用绝对路径 |
| 127 (ERROR_PROC_NOT_FOUND) | 函数找不到 | 确认 DLL 版本正确，函数名拼写无误 |

---

### Q3: C# / Unity 中调用失败

```csharp
// 方法 1：指定完整路径
[DllImport(@"C:\MyApp\checkvm-x64.dll")]

// 方法 2：放到 EXE 同目录，用文件名即可
[DllImport("checkvm-x64.dll")]

// Unity 特殊：放到 Assets/Plugins/x86_64/ 文件夹下
```

---

### Q4: 易语言提示 "找不到 DLL 命令"

1. 确认易语言是 32 位 → 必须用 `checkvm-x86.dll`
2. DLL 放到易语言程序同目录，或放到系统目录（不推荐）
3. 参数必须用 `传址` 修饰，否则栈错误

---

### Q5: 多线程调用安全吗？

**可以**。DLL 内部无全局可变状态，支持多线程并发调用。

---

### Q6: DLL 需要管理员权限吗？

**不需要**。低权限即可运行。

---

### Q7: 怎么确认 DLL 导出的函数正确？

```cmd
dumpbin /exports checkvm-x64.dll
```

应该看到：
```
ordinal hint RVA      name
      1    0 0000B400 ?DetectVm@@...
      2    1 000010B0 DetectVmEx
      3    2 00001040 IsVirtualMachine
```

---

## 五、版本历史

| 版本 | 日期 | 说明 |
|---|---|---|
| 1.0 | 2026-11-03 | 初始版本，3 个导出函数，x64 + x86 |
