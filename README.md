# 2027 哨兵电控工程与公共库

新乡学院 RoboMaster 哨兵电控工程。仓库同时承载 Middleware(面向STM32H723VGT6) 、Device 公共库；公共库按“长期复用、稳定优先、兼容优先”维护。

此工程基于STM32H723VGT6开发，开发平台为达妙MC02开发板。

当前工程仅开启 ICache，DCache 关闭。
关于内存分配请详细见STM32H723XG_FLASH.ld