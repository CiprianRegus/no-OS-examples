#!/bin/sh
#
# Copyright 2024(c) Analog Devices, Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of Analog Devices, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Usage example:
#     ./json2cstring.sh path/to/file.json
#
# Check if the first argument is equal to a specific string
CFILE="${1%.json}.h"
\cp -f $1 $CFILE
sed -i '$ ! s/$/ \\n\\/' $CFILE
sed -i '$s/$/ \\/' $CFILE
sed -i 's/\r//' $CFILE
sed -i 's/\"/\\\"/g' $CFILE
if [ $(basename "$1") = "ActiveUseCase.profile" ]; then
	sed -i '1s/^/const char *json_profile_active_use_case = "/' $CFILE
fi
if [ $(basename "$1") = "ActiveUtilInit.profile" ]; then
	sed -i '1s/^/const char *json_profile_active_util_init = "/' $CFILE
fi
sed -i '$s/ \\/\\n";/' $CFILE
sed -i -e '$a\' $CFILE
echo $CFILE
