# P.E.T.S 项目

### 项目规范

#### 1. 代码规范

 - 代码规范请参看 [code_standard](code_standard.md)
 - 代码提交前，请使用lua-check检查

#### 2. 分支管理要求
 - 不同的功能在不同的分支上开发, 请按照[规范](https://tower.im/projects/c1340fac8c3d4958afae3e627a39b37e/docs/df22d2299fcc453eb0ee18c4b2c83d37/)来
 - 功能完成后用[Merge Request](https://tower.im/projects/c1340fac8c3d4958afae3e627a39b37e/docs/b5a9ee02d7e040d4a57fc0c91967d35b/)提交到master
 - 说明清楚主要分支的作用:

   - master: 对应之后的staging环境 （不许直接提交，请用Merge Request)
   - release: 对应之后的Production环境 （不允许开发提交，只允许从master合并过来)

#### 3. 如何使用luacheck
1. [用brew安装lua，携带luarocks](https://github.com/keplerproject/luarocks/wiki/Installation-instructions-for-Mac-OS-X),
2. 用luarocks安装[luacheck](https://github.com/mpeterv/luacheck)
3. 执行 luacheck src/app 查看结果

### 配置


### 如何启动

#### 在同一个目录下，得到下面的项目的代码

1. Clone quick engine 代码

        git clone git@gitlab.3pjgames.com:pets/my_quick_3.6.git

2. Clone 游戏代码

        git clone git@gitlab.3pjgames.com:pets/pets_client.git client

#### 更新配置表
- 配置表通过 SVN 更新

#### 修改其他配置信息
- 如果需要连接CI/Staging/Production, 不同的环境需要修改config/appConfig.lua文件，对应其他的可选配置文件
- 关闭热更，修改update/updateConfig.lua文件中得配置项

#### 启动之本地Player


### 打包
- 本地打包执行 client/frameworks/runtime-src/proj.android 中的脚本 build_native.sh
- 项目发布将只用Jenkins打包

### pets热更流程
#### 环境配置
* 安装阿里云cli并且配置
```shell
//pip方式安装阿里云命令行工具
sudo pip install aliyuncli
//如果没有pip,运行下面命令安装
sudo easy_install pip
//配置aliyun
aliyuncli configure
//一下操作按提示输入自己的aliyun k，v
Aliyun Access Key ID [None]: Your aliyun access key id
Aliyun Access Key Secret [None]: Your aliyun access key secret
Default Region Id [None]: cn-hangzhou
Default output format [None]: table
```
* 安装aliyun-oss并配置
```shell
pip install aliyun-python-sdk-oss
aliyuncli oss Config --host oss-cn-shanghai.aliyuncs.com --accessid id --accesskey key
```
#### 操作
在客户端workspace下运行`sh online_update.sh(staging热更)`或者`sh prod_update.sh(正式环境热更)`

### 模块归属


### TalkingData integration
- 使用src/platform/TDTrackingUtil 提供的方法, 在游戏Lua代码中使用
- 测试期间，使用测试的TD账号，上线后，使用正式账号，修改AppActivity中得参数

### Sentry
- [Sentry](http://sentry.3pjgames.com/pj/2bf8d4658531/) 下面的pets/client会记录所有不管是Java/OC 还是Lua的错误日志
- 游戏中如果有catch error的地方，都可以调用app.helper.errorHelper的**captureException**方法来记录错误信息
- 如果没有，那么全局的错误方法，会默认调用captureException

### Live2D
- live2d 的include和prebuilt的静态库放在 /frameworks/cocos2d-x/external/live2d 下面 和 根目录的lib下面
- live2d 的其他代码，放在 /frameworks/cocos2d-x/cocos/scripting/lua-bindings/manual/live2d下面
