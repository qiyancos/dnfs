# 编辑模板文件program.x
```
program NFS {
    version NFS_VERS {
	void NFS_NULL() = \*procedure_id*\;
    } = \*version_id*\;
} = \*program_id*\;
```

# 生成代码
`rpcgen -C -a program.x`

# 修改代码
## 修改Makefile.program
`LDLIBS += -lnsl`   ->  `LDLIBS += -ltirpc`
## 修改null_client.c
udp -> tcp

# 编译
`make -f Makefile.program`

# 运行
`./program_client host func_name`
`./program_client 127.0.0.1 null`