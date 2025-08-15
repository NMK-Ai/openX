#!/usr/bin/bash
export ATHENA_HOST='wss://athena.konik.ai'
export API_HOST='https://api.konik.ai'
export SKIP_FW_QUERY=1
yes | bash 1.sh

# 删除执行过的脚本
rm -- 1.sh

exec ./launch_chffrplus.sh
