#!/bin/bash
# ��Ҫ������WEB����

if [[ $# -lt 3 ]];then # ��Ҫָ������������������IP�Ͷ˿�
 echo -ne "Usage :\n\t$0 WORK_PROCESSES LISTEN_ADDRESS LISTEN_PORT [nginx|tengine|openresty] [start|stop|quit|reload]\nExample :\n\t$0 1 0.0.0.0 80\n\t$0 8 0.0.0.0 80 tengine stop\n"
 exit 1
fi

template=/etc/nginx/nginx.conf.template # �뱣֤ģ����ڣ��ű��������
case $4 in  # �Ա� nginx ��tengine �� openresty
    tengine) 
        servername='tengine' ;; 
    openresty) 
        servername='openresty' ;; 
    *) 
        servername='nginx' ;; 
esac   
webserver=/usr/sbin/${servername:=nginx} # Ĭ��ʹ�� nginx
config=/tmp/${servername}_$2_$3_$1.conf

sed "s/WORK_PROCESSES/$1/g" $template | \
sed "s/LISTEN_ADDRESS/$2/g" | \
sed "s/LISTEN_PORT/$3/g" | \
sed "s/ERROR_LOG/$servername/$2_$3_$1_error.log/g" | \
sed "s/PID_NAME/$servername_$2_$3_$1.pid/g" > $config

cmd="$webserver -c $config"
if [[ "x$5" = "x" ]] ||  [[ "x$5" = "xstart" ]] ; then
    runserver="$cmd"
else
    runserver="$cmd -s $5"
fi
$cmd -t && echo -ne "\n\e[0;32m\e[5m$runserver\e[m \n\n" && $runserver