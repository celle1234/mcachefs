/*
 mcachefs -- a caching filesystem to deal with slow filesystem accesses. This
 filesystem assumes that the backing store is fast and local! The filesystem is
 currently read only
 Copyright (C) 2004  Michael Still (mikal@stillhq.com)

 Heavily based on example code that is:
 Copyright (C) 2001  Miklos Szeredi (mszeredi@inf.bme.hu)

 This program can be distributed under the terms of the GNU GPL.
 See the file COPYING.
 */

#include "mcachefs.h"

int
main (int argc, char *argv[])
{
    struct mcachefs_config* config;
    struct mcachefs_metadata_t *mdata_root;

    printf ("mcachefs " __MCACHEFS_VERSION__ " starting up...\n");
    
    config = mcachefs_parse_config(argc, argv);
    
    mcachefs_dump_config(config);
    
    mcachefs_set_current_config(config);


    mcachefs_file_timeslice_init_variables ();

    mcachefs_setstate (MCACHEFS_STATE_NORMAL);
    mcachefs_setwrstate (MCACHEFS_WRSTATE_CACHE);

    mcachefs_metadata_open ();

    mdata_root = mcachefs_metadata_find ("/");
    if (!mdata_root)
    {
        Err ("Could not get metadata for root folder.\n");
        exit (-1);
    }

    memcpy (&mcachefs_target_stat, &(mdata_root->st), sizeof (struct stat));
    mcachefs_metadata_release (mdata_root);

#ifdef MCACHEFS_DISABLE_WRITE
    Info ("Serving read-only !\n");
#else
    Info ("Serving read-write !\n");
#endif

    void *user_data = NULL;
    int fuse_argc = argc -1;
    char** fuse_argv = argv + 1;
    fuse_main (fuse_argc, fuse_argv, &mcachefs_oper, user_data);

    Info ("Serving finished !\n");
    return 0;
}
