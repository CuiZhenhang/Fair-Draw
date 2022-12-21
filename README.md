# 公平抽奖助手

**帮你更轻松地实现公平可信的抽奖。**

遵循 [Apache 2.0](http://www.apache.org/licenses/LICENSE-2.0) 开源协议。

## 如何使用？

### 开始

首先，下载文件到本地。  
请确保你有对应版本的运行环境：
- JavaScript 版本：[node.js](https://nodejs.org/)。
- C++ 版本：64 位 Windows 系统。

然后，双击运行 `run.cmd`。  
如果运行环境正确，你将看到：
```plain
操作模式有：
1. 生成字符串；
2. 生成随机排名；
3. 检验哈希值。
请输入操作模式前的数字：
```

> 注意：输入时**请严格按照提示输入文字，否则程序将报错**。  
> 一个 JavaScript 版本报错的样例如下：
> ```plain
> node:internal/readline/emitKeypressEvents:71
>             throw err;
>             ^
> 
> Error: 输入不合法
>     at D:\Fair-Draw\JavaScript\FairDraw.js:256:28
>     at Interface._onLine (node:readline:485:5)
>     at Interface._line (node:readline:864:8)
>     ......（以下内容省略）
> ```
> 
> 这时，请注意 `Error` 后面的文字，它是报错的原因，帮助我们发现并纠正错误。  
>
> 一个 C++ 版本报错的样例如下：
> ```plain
> terminate called after throwing an instance of 'std::invalid_argument'
>  what():  输入不合法
> ```
> 类似的，请注意 `what()` 后面的文字，它是报错的原因，帮助我们发现并纠正错误。
> 
> 报错发生后，我们可以退出程序，再次运行 `run.cmd`。

> 当程序输出以下内容时：
> ```plain
> 请按任意键继续. . .
> ```
> 这说明程序已经结束了，我们可以退出程序。

> 关于版本的注意事项。
>
> 正常情况下 JavaScript 版本和 C++ 版本的输出是相同的，也就是说可以混用。
>
> 但是，**如果你输入的单个数字达到 $2^{53}$（$9007199254740992$）或输入了负数、小数，输出就不能保证相同。**  
> 如果你输入了非 ASCII 字符（如中文、日文、emoji 等），不同版本的输出**也不能保证相同**。  
> 不过，随机生成的“参数字符串”一定都是 ASCII 字符，所以**正常使用时不必担心**。

### 第一步

第一步时，我们还没有“参数字符串”，所以要生成一个。  
输入 `1` 并回车，程序输出如下内容：
```plain
请输入字符串长度（默认为64）：
```

按提示输入并回车即可。  
但不一样的是，这里在括号中标有默认值，所以我们可以直接按下回车，这相当于输入了默认值。

然后，程序会输出以下内容（每次的结果都不一样）：
```plain
生成的参数字符串为：q6Xb2otbdCYxrDgONV9zcNnuS0aiI3Jj0JahOfjrtaTU5AagwGNF6u1bt36UEQSa
字符串 md5 哈希值为：65c0a0412839cbbd74a527c74c12d991
```

我们复制并保存好“参数字符串”和“md5 哈希值”，然后退出程序。

> 小贴士：复制与粘贴
>
> 在控制台程序中，复制、粘贴与一般程序不同。  
> - 复制时，先选择要复制的文本，然后按下鼠标右键（CTRL+C 快捷键**可能**能用）。
> - 粘贴时，在输入状态下，按下鼠标右键（CTRL+V 快捷键**可能**能用）。如果没有反应，那么可能是粘贴板丢失了，重新复制即可。

### 第二步

在公开发布的抽奖帖子中，我们应当与参与者约定一些变化的数量当作参数（而且约定顺序）。  
这些参数应当满足以下要求：
1. “参数字符串”我们无法随意大量修改。
2. 抽奖截止后不再变化（或变化很小）。
3. 参与者均可见。

像 bilibili 常见的动态抽奖，参数可以使用“参与抽奖总人数”、“抽奖截止时点赞数”等数量。

P.S. 参数可以有多个。

- 另外，在抽奖帖子中，我们**应当**公布“md5 哈希值”，**不应当**公布“参数字符串”。
- 如果我们不是**仅使用第 100 个随机数**（如使用多个随机数），那么我们应当公布它们是第哪些个。
- 为了使参与者明白参数和“md5 哈希值”的意义，我们应当分享该项目的链接。

### 第三步

抽奖截止后，我们（或参与者）收集到了所有参数。  
这时，就可以将参与者数量、“参数字符串”和所有参数公布。  
现在所有人都可以进行验证。

#### 生成随机排名

1. 运行 `run.cmd`，输入 `2` 并回车进入“生成随机排名”模式。
2. 输入参数字符串，并回车。
3. 按照顺序、以空格分隔依次输入所有参数，并回车。
4. 输入参与者数量，并回车。
5. 如果不是**仅使用第 100 个随机数**，以空格分隔依次输入“使用第哪些个随机数”，并回车。
6. 现在程序就会输出最终排名。如果两次输入的内容完全相同，最终排名一定是完全相同的。

一个交互的样例如下：
```plain
操作模式有：
1. 生成字符串；
2. 生成随机排名；
3. 检验哈希值。
请输入操作模式前的数字：2
请输入参数字符串：q6Xb2otbdCYxrDgONV9zcNnuS0aiI3Jj0JahOfjrtaTU5AagwGNF6u1bt36UEQSa
请输入参数列表：233 666
请输入最大排名：131
使用第哪些个随机数（默认仅 100）：20 50 88 33 22
生成的随机排名为：21 52 105 36 60
```

> 注意：生成多个随机数时，它们可能会有重复，但概率较低。

#### 检验哈希值

如果有人怀疑参数字符串被恶意修改了，那么他可以通过检验哈希值判断是否被修改。

1. 运行 `run.cmd`，输入 `3` 并回车进入“检验哈希值”模式。
2. 输入参数字符串，并回车。
3. 输入 md5 哈希值，并回车。
4. 现在程序会输出 md5 哈希值是否正确。如果正确，那么参数字符串是没有被修改的。

一个交互的样例如下：
```plain
操作模式有：
1. 生成字符串；
2. 生成随机排名；
3. 检验哈希值。
请输入操作模式前的数字：3
请输入参数字符串：q6Xb2otbdCYxrDgONV9zcNnuS0aiI3Jj0JahOfjrtaTU5AagwGNF6u1bt36UEQSa
请输入 md5 哈希值：65c0a0412839cbbd74a527c74c12d991
比对成功
```

> 注意：不同的参数字符串拥有相同 md5 哈希值，这种现象一定存在，但随意两个参数字符串发生该现象的概率几乎为 0。  
> 这概率比陌生人一次猜中你超过 20 位长度（假如你有）的密码还低，因此**可以放心**。

### 另一步

排名有了，但怎么确定这个排名是谁呢？

稍微想想，如果名单不能一开始就确定，那么这里也可能存在不公平。

这里提供一种方案：  
使用 Excel 表格将参与者按照名称（或 uid、QQ 等具有辨识度的信息）排序。  
如何排序，也要提前约定。  
排序的结果推荐公布，方便参与者检查。
