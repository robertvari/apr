/* ====================================================================
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2000 The Apache Software Foundation.  All rights
 * reserved.
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
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Apache" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#include <stdio.h>
#include "apr_file_io.h"
#include "apr_errno.h"
#include "apr_general.h"
#include "apr_lib.h"
#include <stdlib.h>
#ifdef BEOS
#include <unistd.h>
#endif

int main(void)
{
    ap_pool_t *context;
    ap_file_t *readp = NULL;
    ap_file_t *writep = NULL;
    ap_size_t nbytes;
    ap_status_t rv;
    char *buf;
    char msgbuf[120];

    if (ap_initialize() != APR_SUCCESS) {
        fprintf(stderr, "Couldn't initialize.");
        exit(-1);
    }
    atexit(ap_terminate);
    if (ap_create_pool(&context, NULL) != APR_SUCCESS) {
        fprintf(stderr, "Couldn't allocate context.");
        exit(-1);
    }

    fprintf(stdout, "Testing pipe functions.\n");

    fprintf(stdout, "\tCreating pipes.......");
    if ((rv = ap_create_pipe(&readp, &writep, context)) != APR_SUCCESS) {
        fprintf(stderr, "ap_create_pipe()->%d/%s\n",
                rv, ap_strerror(rv, msgbuf, sizeof msgbuf));
        exit(-1);
    }
    else {
        fprintf(stdout, "OK\n");
    }
    
    fprintf(stdout, "\tSetting pipe timeout.......");
    if ((rv = ap_set_pipe_timeout(readp, 1 * AP_USEC_PER_SEC)) != APR_SUCCESS) {
        fprintf(stderr, "ap_set_pipe_timeout()->%d/%s\n",
                rv, ap_strerror(rv, msgbuf, sizeof msgbuf));
        exit(-1);
    } else {
        fprintf(stdout, "OK\n");
    }        

    fprintf(stdout, "\tReading from the pipe.......");
    nbytes = (ap_ssize_t)strlen("this is a test");
    buf = (char *)ap_palloc(context, nbytes + 1);
    if (ap_read(readp, buf, &nbytes) == APR_TIMEUP) {
        fprintf(stdout, "OK\n");
    }
    else {
        fprintf(stdout, "The timeout didn't work  :-(\n");
        exit(-1);
    }

    return 1;
}
