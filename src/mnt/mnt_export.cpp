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
#include "mnt/mnt_export.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

//static bool proc_export(struct gsh_export *export, void *arg)
//{
//    struct proc_state *state = arg;
//    struct exportnode *new_expnode;
//    struct glist_head *glist_item;
//    struct groupnode *group, *grp_tail = nullptr;
//    char *grp_name;
//    bool free_grp_name;
//    struct glist_head *clients = &export->clients;
//
//    state->retval = 0;
//
//    /* If client does not have any access to the export,
//     * don't add it to the list
//     */
//    get_gsh_export_ref(export);
//    set_op_context_export(export);
//    export_check_access();
//
//    if (!(op_ctx->export_perms.options & EXPORT_OPTION_ACCESS_MASK)) {
//        LogFullDebug(COMPONENT_NFSPROTO,
//                     "Client is not allowed to access Export_Id %d %s",
//                     export->export_id, ctx_export_path(op_ctx));
//
//        goto out;
//    }
//
//    if (!(op_ctx->export_perms.options & EXPORT_OPTION_NFSV3)) {
//        LogFullDebug(COMPONENT_NFSPROTO,
//                     "Not exported for NFSv3, Export_Id %d %s",
//                     export->export_id, ctx_export_path(op_ctx));
//
//        goto out;
//    }
//
//    new_expnode = gsh_calloc(1, sizeof(struct exportnode));
//
//    PTHREAD_RWLOCK_rdlock(&op_ctx->ctx_export->exp_lock);
//    PTHREAD_RWLOCK_rdlock(&export_opt_lock);
//
//    if (glist_empty(clients))
//        clients = &export_opt.clients;
//
//    glist_for_each(glist_item, clients) {
//        struct base_client_entry *client =
//                glist_entry(glist_item,
//        struct base_client_entry,
//        cle_list);
//
//        group = gsh_calloc(1, sizeof(struct groupnode));
//
//        if (grp_tail == nullptr)
//            new_expnode->ex_groups = group;
//        else
//            grp_tail->gr_next = group;
//
//        grp_tail = group;
//        free_grp_name = false;
//        switch (client->type) {
//            case NETWORK_CLIENT:
//                grp_name = cidr_to_str(client->client.network.cidr,
//                                       CIDR_NOFLAGS);
//                if (grp_name == nullptr) {
//                    state->retval = errno;
//                    grp_name = "Invalid Network Address";
//                } else {
//                    free_grp_name = true;
//                }
//                break;
//            case NETGROUP_CLIENT:
//                grp_name = client->client.netgroup.netgroupname;
//                break;
//            case GSSPRINCIPAL_CLIENT:
//                grp_name = client->client.gssprinc.princname;
//                break;
//            case MATCH_ANY_CLIENT:
//                grp_name = "*";
//                break;
//            case WILDCARDHOST_CLIENT:
//                grp_name = client->client.wildcard.wildcard;
//                break;
//            default:
//                grp_name = "<unknown>";
//        }
//        LogFullDebug(COMPONENT_NFSPROTO,
//                     "Export %s client %s",
//                     ctx_export_path(op_ctx), grp_name);
//        group->gr_name = gsh_strdup(grp_name);
//        if (free_grp_name)
//            free(grp_name);
//    }
//
//    PTHREAD_RWLOCK_unlock(&export_opt_lock);
//    PTHREAD_RWLOCK_unlock(&op_ctx->ctx_export->exp_lock);
//
//    /* Now that we are almost done, get a gsh_refstr to the path for ex_dir.
//     * The op context has a reference but we don't want to play games with
//     * it to keep the code understandable.
//     */
//    if (nfs_param.core_param.mount_path_pseudo)
//        new_expnode->ex_refdir = gsh_refstr_get(op_ctx->ctx_pseudopath);
//    else
//        new_expnode->ex_refdir = gsh_refstr_get(op_ctx->ctx_fullpath);
//
//    new_expnode->ex_dir = new_expnode->ex_refdir->gr_val;
//
//    if (state->head == nullptr)
//        state->head = new_expnode;
//    else
//        state->tail->ex_next = new_expnode;
//
//    state->tail = new_expnode;
//
//    out:
//    clear_op_context_export();
//    return true;
//}

/**
 * @brief The Mount proc EXPORT function, for all versions.
 *
 * Return a list of all exports and their allowed clients/groups/networks.
 *
 * @param[in]  arg     Ignored
 * @param[in]  req     Ignored
 * @param[out] res     Pointer to the export list
 *
 */

int mnt_export(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
//    struct proc_state proc_state{};
//
//    /* init everything of interest to good state. */
//    memset(res, 0, sizeof(nfs_res_t));
//    memset(&proc_state, 0, sizeof(proc_state));
//
//    (void)foreach_gsh_export(proc_export, false, &proc_state);
//    if (proc_state.retval != 0) {
//        LogCrit(COMPONENT_NFSPROTO,
//                "Processing exports failed. error = \"%s\" (%d)",
//                strerror(proc_state.retval), proc_state.retval);
//    }
//    res->res_mntexport = proc_state.head;
    return NFS_REQ_OK;
}				/* mnt_Export */

/**
 * mnt_Export_Free: Frees the result structure allocated for mnt_Export.
 *
 * Frees the result structure allocated for mnt_Dump.
 *
 * @param res	[INOUT]   Pointer to the result structure.
 *
 */
void mnt_export_free(nfs_res_t *res)
{
//    struct exportnode *exp, *next_exp;
//    struct groupnode *grp, *next_grp;
//
//    exp = res->res_mntexport;
//    while (exp != nullptr) {
//        next_exp = exp->ex_next;
//        grp = exp->ex_groups;
//        while (grp != nullptr) {
//            next_grp = grp->gr_next;
//            if (grp->gr_name != nullptr)
//                free(grp->gr_name);
//            free(grp);
//            grp = next_grp;
//        }
//        free(exp);
//        exp = next_exp;
//    }
}				/* mnt_Export_Free */
