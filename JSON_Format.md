# Client->Server Json介绍

Basic attributes:
``` json
{
    // 可为 1, 2, 3 分别对应主界面三个按钮，3对应自定义动作
    "type":1,

    // 间隔时间 单位ms
    "interval":500,
    "height":800,
    "width":600
}
```

type1,type2为默认参数，无需client重复发送
type3时需要提供额外属性如下
``` json
{
    // 对应文档顺序的5个角度，浮点型保存，角度值为0~180.0
    "angle":[1.0, 2.0, 3.0, 4.0, 5.0],

    // 持续时间，单位ms 整形
    "stay": 1500
}
```


# Server->Client Json介绍
``` json
{
    // 标志当前动作是否标准，不考虑维持时间
    "correct": true,
    // 标志动作持续时间是否达到,达到后持续返回true
    // 考虑到重复做动作，若correct持续5秒false server将此flag标为false
    "done": false,
    // 需要显示到平面上的消息
    "msg": "fsdfdsfdsfs"
}
```