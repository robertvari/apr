/* ====================================================================
 * Copyright (c) 1999 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */

#include "mm.h"
#include "apr_general.h"
#include "apr_shmem.h"
#include "apr_errno.h"

struct shmem_t {
    MM *mm;
};

/* ***APRDOC********************************************************
 * ap_status_t ap_shm_init(ap_shmem_t *, ap_size_t, char *)
 *    Create a pool of shared memory for use later.
 * arg 1) The shared memory block.
 * arg 2) The size of the shared memory pool.
 * arg 3) The file to use for the shared memory on platforms that
 *        require it.
 */
ap_status_t ap_shm_init(struct shmem_t **m, ap_size_t reqsize, const char *file)
{
    MM *newmm = mm_create(reqsize, file);
    if (newmm == NULL) {
        return errno;
    }
    (*m) = mm_malloc(newmm, sizeof(struct shmem_t));
    (*m)->mm = newmm;
    return APR_SUCCESS;
}

/* ***APRDOC********************************************************
 * ap_status_t ap_shm_destroy(ap_shmem_t *)
 *    Destroy the shared memory block.
 * arg 1) The shared memory block to destroy. 
 */
ap_status_t ap_shm_destroy(struct shmem_t *m)
{
    mm_destroy(m->mm);
    return APR_SUCCESS;
}

/* ***APRDOC********************************************************
 * ap_status_t ap_shm_malloc(ap_shmem_t *, ap_size_t)
 *    allocate memory from the block of shared memory.
 * arg 1) The shared memory block to destroy. 
 * arg 2) How much memory to allocate
 */
void *ap_shm_malloc(struct shmem_t *c, ap_size_t reqsize)
{
    if (c->mm == NULL) {
        return NULL;
    }
    return mm_malloc(c->mm, reqsize);
}

/* ***APRDOC********************************************************
 * void *ap_shm_calloc(ap_shmem_t *, ap_size_t)
 *    allocate memory from the block of shared memory and initialize it
 *    to zero.
 * arg 1) The shared memory block to destroy. 
 * arg 2) How much memory to allocate
 */
void *ap_shm_calloc(struct shmem_t *shared, ap_size_t size) 
{
    if (shared == NULL) {
        return NULL;
    }
    return mm_calloc(shared->mm, 1, size);
}

/* ***APRDOC********************************************************
 * ap_status_t ap_shm_free(ap_shmem_t *, void *)
 *    free shared memory previously allocated.
 * arg 1) The shared memory block to destroy. 
 */
ap_status_t ap_shm_free(struct shmem_t *shared, void *entity)
{
    mm_free(shared->mm, entity);
    return APR_SUCCESS;
}

/* ***APRDOC********************************************************
 * ap_status_t ap_get_shm_name(ap_shmem_t *, ap_shm_name_t **)
 *    Get the name of the shared memory segment if not using
 *    anonymous shared memory.
 * arg 1)  The shared memory block to destroy. 
 * arg 2)  The name of the shared memory block, NULL if anonymous 
 *         shared memory.
 * return) APR_USES_ANONYMOUS_SHM if we are using anonymous shared
 *         memory.  APR_USES_FILEBASED_SHM if our shared memory is
 *         based on file access.  APR_USES_KEYBASED_SHM if shared
 *         memory is based on a key value such as shmctl.  If the
 *         shared memory is anonymous, the name is NULL.
 */
ap_status_t ap_get_shm_name(ap_shmem_t *c, ap_shm_name_t **name)
{
#if APR_USES_ANONYMOUS_SHM
    name = NULL;
    return APR_ANONYMOUS;
/* Currently, we are not supporting name based shared memory on Unix
 * systems.  This may change in the future however, so I will leave
 * this in here for now.  Plus, this gives other platforms a good idea
 * of how to proceed.
 */
#elif APR_USES_FILEBASED_SHM
#elif APR_USES_KEYBASED_SHM
#endif
}

/* ***APRDOC********************************************************
 * ap_status_t ap_set_shm_name(ap_shmem_t *, ap_shm_name_t *)
 *    Set the name of the shared memory segment if not using
 *    anonymous shared memory.  This is to allow processes to open
 *    shared memory created by another process.
 * arg 1)  The shared memory block to destroy. 
 * arg 2)  The name of the shared memory block, NULL if anonymous 
 *         shared memory.
 * return) APR_USES_ANONYMOUS_SHM if we are using anonymous shared
 *         memory.  APR_SUCCESS if we are using named shared memory
 *         and we were able to assign the name correctly. 
 */
ap_status_t ap_set_shm_name(ap_shmem_t *c, ap_shm_name_t *name)
{
#if APR_USES_ANONYMOUS_SHM
    return APR_ANONYMOUS;
/* Currently, we are not supporting name based shared memory on Unix
 * systems.  This may change in the future however, so I will leave
 * this in here for now.  Plus, this gives other platforms a good idea
 * of how to proceed.
 */
#elif APR_USES_FILEBASED_SHM
#elif APR_USES_KEYBASED_SHM
#endif
}

/* ***APRDOC********************************************************
 * ap_status_t ap_open_shmem(ap_shmem_t *)
 *    Open the shared memory block in a child process. 
 * arg 1)  The shared memory block to open in the child. 
 * return) This should be called after ap_set_shm_name.  The ap_shmem_t 
 *         variable must refer to the memory segment to open.
 */
ap_status_t ap_open_shmem(struct shmem_t *c)
{
#if APR_USES_ANONYMOUS_SHM
/* When using MM, we don't need to open shared memory segments in child
 * segments, so just return immediately.
 */
    return APR_SUCCESS;
/* Currently, we are not supporting name based shared memory on Unix
 * systems.  This may change in the future however, so I will leave
 * this in here for now.  Plus, this gives other platforms a good idea
 * of how to proceed.
 */
#elif APR_USES_FILEBASED_SHM
#elif APR_USES_KEYBASED_SHM
#endif
}

/* ***APRDOC********************************************************
 * ap_status_t ap_shm_avail(ap_shmem_t *, ap_size_t *)
 *    Determine how much memory is available in the specified shared
 *    memory block
 * arg 1)  The shared memory block to open in the child. 
 * arg 2)  The amount of space available in the shared memory block.
 */
ap_status_t ap_shm_avail(struct shmem_t *c, ap_size_t *size)
{
    *size = mm_available(c);
    if (*size == 0) {
        return APR_ESHMLOCK;
    }
    return APR_SUCCESS;
}
