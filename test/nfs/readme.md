1. 编辑模板文件program.x（无改动时可跳过）
```
program NFS {
    version NFS_VERS {
	void NFS_NULL() = \*procedure_id*\;
    } = \*version_id*\;
} = \*program_id*\;
```

2. 生成代码（无改动时可跳过）
`rpcgen -C -a program.x`

3. 根据需求修改代码
比如 program_client.c 中， udp改tcp

4. 编译
修改Makefile.program，`LDLIBS += -lnsl`改为`LDLIBS += -ltirpc` 
执行 `make -f Makefile.program`

5. 运行
客户端：`./program_client 127.0.0.1`
服务端：`./program_server`
