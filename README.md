# DirectXAssistant

**前身是`Graphics2D`库，重新组织了代码**

`FillBitmap`方法：在指定rectangle中绘制bitmap，铺满rectangle，不拉伸，多余部分会被裁剪

`PutBitmap`方法：在指定rectangle中绘制bitmap，不拉伸，保证整张bitmap在rectangle中，实际绘制区rectangle可能比提供的小，返回实际rectangle
