#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>

//#define my_device "88:79:7E:2B:C1:25"
#define my_device "78:4F:43:AF:1F:B1"
#define nikola_pc "9C:B6:D0:8C:4C:78"

char* Scan_device(int dev_id,int sock)
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int len = 8, flags = IREQ_CACHE_FLUSH;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    max_rsp = 255;

   ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
while(1)
  {
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);//performs a Bluetooth device discovery and returns a list of detected devices and some basic information about them in the variable ii.
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++)
    {

      ba2str(&(ii+i)->bdaddr, addr);

        memset(name, 0, sizeof(name));

        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0)
        strcpy(name, "[unknown]");
        printf("%s  %s\n", addr, name);
	if((strcmp(addr,my_device) == 0))
        {
		break;
        }
    }
    if((strcmp(addr,my_device) == 0))
    {
      break;
    }
  } 
    free( ii );


char* data = NULL;
	if(addr){
		data = (char*)malloc(sizeof(char)*20);
		memset(data,0,sizeof(data));
		sprintf(data,"%s",addr);
	}
    return data;
}

int bluez_pair_device(char* addr, char *controller_baddr)
{
    char btaddr_str[19] = { '\0' };
    int retry_count = 0;
    char pair_cmd[512];
    char response[512];
    char keycmd[512];
    char hciinfo[50];
    FILE *fp;
    int ret;
    strcpy(btaddr_str, addr);

    sprintf(keycmd, "sudo cat /var/lib/bluetooth/%s/%s/info | grep Key=", controller_baddr, btaddr_str);
    fp = popen(keycmd, "r");
    if (fp) {
        if (fgets(response, sizeof(response) - 1, fp) != NULL) {
            pclose(fp);
            goto exit_pair;
        }
        printf("Pairing does not exists for: '%s'", btaddr_str);
        pclose(fp);
    }

    strcpy(response, btaddr_str);
    btaddr_str[2] = '_';
    btaddr_str[5] = '_';
    btaddr_str[8] = '_';
    btaddr_str[11] = '_';
    btaddr_str[14] = '_';
    sprintf(pair_cmd, "sudo -i hcitool cc %s && dbus-send --system --dest=org.bluez --print-reply /org/bluez/hci0/dev_%s org.bluez.Device1.Pair",
        response,
        btaddr_str);

    sprintf(hciinfo, "hcitool info %s", response);
	
retry_pair:
    if (retry_count > 5)
        return -1;

    ret = system(pair_cmd);
    if (ret) {
        retry_count++;
        printf("%s failed to pair BT device: %s", __FUNCTION__, btaddr_str);
        printf("cmd: '%s'", pair_cmd);
        system(hciinfo);
    }

    fp = popen(keycmd, "r");
    if (!fp) {
        printf("%s failed to run cmd: '%s'", __FUNCTION__, keycmd);
        retry_count++;
        goto retry_pair;
    }
    if (fgets(response, sizeof(response) - 1, fp) == NULL) {
        pclose(fp);
        printf("%s LinkKey missing in file", __FUNCTION__);
        printf("cmd: '%s'", pair_cmd);
        retry_count++;
        goto retry_pair;
    }
    pclose(fp);

exit_pair:
    return 0;
}

void bluetooth_receive(){

    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[30] = { 0 };
    int s,client, bytes_read;
    socklen_t opt = sizeof(rem_addr);
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
while(buf[0] != '0'){
    memset(buf, 0, sizeof(buf));

    // read data from the client
    bytes_read = read(client, buf, sizeof(buf));
    if( bytes_read > 0 ) {
        printf("received [%s]\n", buf);
    }
}
    // close connection
    close(client);
    close(s);
}

void bluetooth_send(){
	printf("\n Line == %d \n",__LINE__);
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[30] = { 0 };
    int s,client, bytes_read;
    socklen_t opt = sizeof(rem_addr);
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
    while(buf[0] != '0' || client == 0){
    memset(buf, 0, sizeof(buf));
	printf("\n Enter value \n");
    // write data from the client
    gets(buf);
    write(client,buf,sizeof(buf));
    }

    // close connection
   close(client);
   close(s);
}

void bluetooth_remove(){
	system("./command.sh");
	printf("\n ------Unpaired bluetooth device ----------------- \n");	
}

void bluetooth_connect(){
	struct sockaddr_rc addr = { 0 };
	int s, status, len=0;
	char buf[256];
	
	// allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
 
	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	str2ba( my_device, &addr.rc_bdaddr );
 
	// connect to server
	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

 
	if(status)
	{
		printf(" failed to connect the device!\n");
		return -1;
	}
 
	do
	{
		printf("Connected...\n");    
		len = read(s, buf, sizeof buf);
 
		if( len>0 ) 
		{
       	
			buf[len]=0;
			printf("%s\n",buf);
			write(s, buf, strlen(buf)); 
		
		}
	} while(len>0); 
   
	close(s);
	

}
int main(){
	int dev_id,sock,status;
	char* ip_address;
start_pair:
	 dev_id = hci_get_route(NULL); // retrieve the resource number of the first available Bluetooth adapter.
	 sock = hci_open_dev( dev_id );// convenience function that opens a Bluetooth socket with the specified resource number.
	 if (dev_id < 0 || sock < 0) {
        	perror("opening socket");
	        exit(1);
	 }
	
	ip_address = Scan_device(dev_id,sock);
	printf(" \n %s \n",ip_address);
	bluez_pair_device(ip_address,nikola_pc);
	int val = -1;


while(val != 0){

	printf("\n\n Select 1: Send \n Select 2: Receive \n Select 3: Removed paired device \n Select 4: re-pair device \n Select 5: connect device \n Select 0: Exit \n");
	scanf("%d",&val);  
	switch(val){
	case 1:
		bluetooth_send();
		break;
	case 2:
		bluetooth_receive();	
		break;
	case 3:
		bluetooth_remove();	
		break;
	case 4:
		goto start_pair;
		break;
        case 5:
                bluetooth_connect();
                break;
	case 0:
		val = 0;
		break;
	default:{
		printf("\n Exit \n");
		val = 0;
		break;
		}
	}
}
	close( sock );
	free(ip_address);

return 0;
}
