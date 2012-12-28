Status(){// Run-Time Settings (F4) --> "Additional attribules"
    char * tname;

    GetIP((int)lr_get_attrib_long("cIP"));
 	GetPORT((int)lr_get_attrib_long("cPort"));
    tname = lr_eval_string("{IP}_{PORT}");

	lr_start_transaction(tname);

	web_url("status",
		"URL=http://{IP}:{PORT}/status",
//		"URL=http://10.20.216.134:8000/status",
		"Resource=0","RecContentType=text/html","Mode=HTTP",LAST );

	lr_end_transaction(tname, LR_AUTO);

}

const static char ip[4][16] = {"10.20.216.130","10.20.216.133","10.20.216.132","10.20.216.134"};

char* GetIP(int t){ 
	switch (t) {
	case 0:
		lr_save_string(ip[0],"IP"); // eth0
		break;
	case 1:
		lr_save_string(ip[1],"IP"); // eth1
		break;
	case 2:
		lr_save_string(ip[2],"IP"); // eth2
		break;
	case 3:
		lr_save_string(ip[3],"IP"); // eth3
		break;
	case 4:
		lr_save_string(ip[rand()%3],"IP"); // rand eth0~2
		break;
	default:
		lr_save_string(ip[rand()%4],"IP"); // rand eth0~3
		break;
	}
}

int GetPORT(int port){ // 24颗CPU启动了24个进程
	switch (port) {
	case 0:// 7001~7024
		return lr_save_int(7001 + rand() % 24,"PORT");
	case 1:// 8001~8024
		return lr_save_int(8001 + rand() % 24,"PORT");
	case 2:// 9001~9024
		return lr_save_int(9001 + rand() % 24,"PORT");
	default:
		return lr_save_int(port,"PORT");
	}
}

Demo(){
	char * r = {0};

	web_add_header("Host","app.weikan.cn");
	web_reg_save_param("WholeBody", "LB=", "RB=", "Search=Body","Notfound=warning", LAST );
	lr_start_transaction("Query");

	web_custom_request("o",
		"URL=http://192.168.1.119/status",
		"Body=", "Mode=HTTP",LAST);

	r = lr_eval_string("{WholeBody}");

	if (strncmp(r,"{\"ret\":0,\"data\":",9) == 0) {
		lr_end_transaction("Query", LR_AUTO);
	}else{
		lr_end_transaction("Query", LR_AUTO);
		lr_error_message("[%d]:%s",strlen(r),r);
	}
}