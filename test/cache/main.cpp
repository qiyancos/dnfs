/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */
#include "cache/bin_log.h"
#include <cstring>
int main() {
    auto *a = new LogBufferMap();
    ObjectHandle *s = (ObjectHandle *) malloc(sizeof(ObjectHandle));
    ObjectInfoBase *w = (ObjectInfoBase *) malloc(sizeof(ObjectInfoBase));
    ObjectInfoBase *j = (ObjectInfoBase *) malloc(sizeof(ObjectInfoBase));
    a->insert({s, w});
    printf("%p\n",a->find(s)->second);
    auto n=a->find(s);
    n->second=j;
    printf("%p\n",a->find(s)->second);
    printf("%p\n",j);
    auto *p=new LogBufferMap();

    auto *o=new LogBufferMap();
    for (auto &data: *a) {
        p->insert({data.first,data.second});
    }
    n->second=w;
    printf("%p\n",p->find(s)->second);
    printf("%p\n",a->find(s)->second);

    memcpy(p,a,a->size()*(sizeof(ObjectInfoBase*)+ sizeof(ObjectHandle*)));
    printf("%d\n",p->size());
    printf("%d\n", a->size());
    free(s);
    free(w);
    delete a;
    return 0;
}