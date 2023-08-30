# 编辑模板文件test.x
```
program NFS {
    version NFS_VERS {
	void NFS_NULL() = \*procedure_id*\;
    } = \*version_id*\;
} = \*program_id*\;
```

# 生成代码
`rpcgen -C -a null.x`

# 修改代码
## 修改Makefile.null
`LDLIBS += -lnsl`   ->  `LDLIBS += -ltirpc`
## 修改null_client.c
udp -> tcp

# 编译
`make -f Makefile.null`

# 运行
`./null_client 127.0.0.1`