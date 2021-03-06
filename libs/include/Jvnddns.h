#ifndef _JVNDDNS_H_
#define _JVNDDNS_H_

#define JVN_DDNS_DYNDNS    	1    	//Dyndns DDNS 开放
#define JVN_DDNS_QDNS    		3    	//3322 DDNS 开放
#define JVN_DDNS_NOIP      	5    	//NO-IP DDNS 开放
#define JVN_DDNS_ORAY			7	  	//oray花生壳 DDNS 开放
#define JVN_DDNS_MYQSEE			9	  	//Myq-see DDNS 开放
#define JVN_DDNS_PEOPLEFU		11   	//peoplefu.com 客户定制类型，不具有开放性
#define JVN_DDNS_APRESS			13		//a-press	DDNS 客户私有定制 星远威视
#define JVN_DDNS_HECKER			15		//heckerddns DDNS 开放

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                    								                                        //
//                     JVNDDNS接口                              							                            //
//                                                                                                     								                                    //
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/***************************************接口清单*****************************
Jvnddns_Init -----------------01 	初始化DDNS库资源
Jvnddns_StartDdns ------------02		开启DDNS域名服务
Jvnddns_EnableLog-------------03		设置错误日志打印使能
Jvnddns_Stop------------------04		关闭DDNS域名服务
Jvnddns_RegCallback-----------05		回调函数注册
Jvnddns_Release --------------06		释放DDNS库资源
Jvnddns_GetVer ---------------07		获取版本号
Jvnddns_SetUpdateEveryTime----08		设置更新模式
***************************************************************************/ 

/****************************************************************************
*名    称：	回调函数类型
*功    能：	定义回调函数
*参    数：	无
*返回值：	无
*其    他：	无
****************************************************************************/
typedef void (*ddns_callback)(int, void *);

/***************************************************************************
*名    称：	Jvnddns_Init
*功    能：	获取公网IP，设置公网IP更新周期
*参    数：	[IN]	nSecond 		IP上传更新周期,不可小于10秒，建议不小于20s
*返回值：	0：成功
			非0：初始化失败	
*其    他：	nSecond参数--更新时间为秒级///
			0表示Jvnddns_StartDdns运行一次则IP更新一次，
			大于0则Jvnddns_StartDdns运行后按该周期更新外网ip
****************************************************************************/
int Jvnddns_Init(int nSecond);//

/****************************************************************************
*名    称： 	JVN_StartDdns
*功    能： 	开启DDNS域名服务功能 
*参    数：  	 [IN]	ddnsType  	DDNS类型
			[IN]	ddnsHost		本地域名（类型ddnsType为JVN_DDNS_PEOPLEFU时，该参数为dyndns.com; peoplefu.com; techonecctv.com）
			[IN]	ddnsUser		用户名
			[IN]	ddnsPwd		密码
			[IN]	netCard		类似 eth0 eth1...的字符串
			[IN]	chSerialNum 设备ID或设备序列号（类型ddnsType为JVN_DDNS_PEOPLEFU时，该参数为必需，最大64位，其他类型时该参数为空即可）
*返回值： 	0：成功
			>0：失败，请检查传入参数	
*其    他： 	调用后上传更新一次ip，检测是否设置更新周期，如果设置，则定时更新，
			否则程序退出，下次调用时有效
			类型JVN_DDNS_PEOPLEFU为客户定制ddns,
****************************************************************************/
int Jvnddns_StartDdns(int ddnsType, char *ddnsHost, char *ddnsUser, 
							 char *ddnsPwd, char *netCard, char *chSerialNum);

/****************************************************************************
*名    称：	Jvnddns_EnableLog
*功    能：	设置写出错日志是否有效
*参    数：	[IN] 	nEnable  1:出错时写日志；0:不写任何日志
*返回值：	成功
*其    他：	无
****************************************************************************/
void Jvnddns_EnableLog(int nEnable);

/****************************************************************************
*名    称：	Jvnddns_Stop
*功    能：	关闭DDNS域名服务功能
*参    数：	无
*返回值：	0：成功
			>0：失败
*其    他：	无
****************************************************************************/
int Jvnddns_Stop();

/****************************************************************************
*名    称：	Jvnddns_RegCallback
*功    能：	回调函数注册，默认为NULL
*参    数：	[IN]	函数指针
*返回值：	无
*其    他：	回调函数中参数到返回值见最后说明
****************************************************************************/
void Jvnddns_RegCallback(ddns_callback);

/****************************************************************************
*名    称：	Jvnddns_Release
*功    能：	释放库资源
*参    数：	无
*返回值：	0：成功
*其    他：	该函数采用异步模式，调用后立即返回，
			但是资源到完全释放会在初始化到时间间隔内完成。
****************************************************************************/
int Jvnddns_Release();

/****************************************************************************
*名    称：	Jvnddns_GetVer
*功    能：	获取当前版本号
*参    数：	无
*返回值：	返回版本号，1009
*其    他：	
****************************************************************************/
int Jvnddns_GetVer();

/****************************************************************************
*名    称：	Jvnddns_SetUpdateEveryTime
*功    能：	设置外网IP地址未变时依然到DDNS服务器更新，
*参    数：	[IN]	nUpdate	0外网IP改变时更新，1每次均更新
*返回值：	0 设置成功  -1设置失败
*其    他：	初始化后调用，（原则上IP地址未变，无需访问DDNS服务器更新，特殊客户则要求IP地址不变时亦更新）
****************************************************************************/
int Jvnddns_SetUpdateEveryTime(int nUpdate);


/*********************回调函数返回值:int char* 含义*************************
*	0		更新成功
*	1		用户名/密码/域名错误
*	2		其他错误
***************************************************************************/

#endif
