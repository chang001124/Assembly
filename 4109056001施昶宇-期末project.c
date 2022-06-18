#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/************************************/
FILE *pass1,*symbol,*pass2,*record;
/************************************/
typedef struct opcode *opnode;
typedef struct opcode{
	char name[10];
	char number[2];
	opnode link;
}opcode;
opnode opcode_root=NULL;
/************************************/
typedef struct source *source_node;
typedef struct source{
	char location[5];
	char labelname[10];
	char opcode[10];
	char name[10];
	char object_code[6];
	source_node link;
}source;
source_node source_root=NULL;
/************************************/
void opcode_creat(char name[],char number[]){
	opnode new_one=(opnode)malloc(sizeof(opcode));
	strcpy(new_one->name,name);
	strcpy(new_one->number,number);
	new_one->link=NULL;
	if(!opcode_root){
		opcode_root=new_one;
		return;
	}
	opnode current=opcode_root;
	while(current->link!=NULL)
		current=current->link;
	current->link=new_one;
}
/************************************/
void show_opcode(){
	opnode current=opcode_root;
	while(current!=NULL){
		printf("%s %s\n",current->name,current->number);
		current=current->link;
	}
}
/************************************/
void source_creat(char labelname[],char opcode[],char name[]){
	source_node new_one=(source_node)malloc(sizeof(source));
	strcpy(new_one->location,"0000");
	strcpy(new_one->object_code,"000000");
	strcpy(new_one->labelname,labelname);
	strcpy(new_one->opcode,opcode);
	strcpy(new_one->name,name);
	new_one->link=NULL;
	if(!source_root){
		source_root=new_one;
		return;
	}
	source_node current=source_root;
	while(current->link!=NULL)
		current=current->link;
	current->link=new_one;
}
/************************************/
int check(char name[]){
		opnode current=opcode_root;
		while(current){//if name==opcode return 1
			if(!strcmp(current->name,name)||!strcmp(name,"START")||!strcmp(name,"RSUB")||!strcmp(name,"BYTE")||!strcmp(name,"WORD")||!strcmp(name,"RESB")||!strcmp(name,"RESW")||!strcmp(name,"END"))
				return 1;
			current=current->link;
		}
	return 0;
} 
/************************************/
void show_source(){
	source_node current=source_root;
	pass1=fopen("pass1.txt","w");
	while(current!=NULL){
		fprintf(pass1,"%s	",current->location);
		fprintf(pass1,"%s	%s	%s\n",current->labelname,current->opcode,current->name);
		current=current->link;
	}
}
/************************************/
void reset(char arr[]){
	memset(arr,0,strlen(arr));//reset array
}
/************************************/
int hextodec(char num[]){
	int index=0,i,count=1,temp;
	for(i=strlen(num)-1;i>=0;i--){
		switch(num[i]){
			case 'A': temp=10; break;
			case 'B': temp=11; break;
			case 'C': temp=12; break;
			case 'D': temp=13; break;
			case 'E': temp=14; break;
			case 'F': temp=15; break;
			default :temp=num[i]-'0'; break;
		}
		index+=temp*count;
		count*=16;
	}
	return index;
}
/************************************/
void dectohex(int num,char hex[]){
	int a=num,i=3;
	strcpy(hex,"0000");
	while(num){
		a=num%16;
		switch(a){
			case 0: hex[i]='0'; break;
			case 1: hex[i]='1'; break;
			case 2: hex[i]='2'; break;
			case 3: hex[i]='3'; break;
			case 4: hex[i]='4'; break;
			case 5: hex[i]='5'; break;
			case 6: hex[i]='6'; break;
			case 7: hex[i]='7'; break;
			case 8: hex[i]='8'; break;
			case 9: hex[i]='9'; break;
			case 10: hex[i]='A'; break;
			case 11: hex[i]='B'; break;
			case 12: hex[i]='C'; break;
			case 13: hex[i]='D'; break;
			case 14: hex[i]='E'; break;
			case 15: hex[i]='F'; break;
			default:  break;
		}
		i--;
		num/=16;
	}
}
/************************************/
void loc(){
	char num[4],hex[4];
	source_node current=source_root;
	while(current->link!=NULL){
		if(!strcmp(current->opcode,"START")){//opcode==START
			int i,j=3;
			for(i=strlen(current->name)-1;i>=0;i--){
				current->location[j]=current->name[i];
				current->link->location[j--]=current->name[i];
			}
			strcpy(num,current->name);
			current=current->link;
			break;
		}
		current=current->link;
	}
	while(current->link!=NULL){
		current=current->link;
		if(!strcmp(current->opcode,"RESB")){//opcode==RESB
			dectohex(hextodec(num)+3,hex);
			strcpy(current->location,hex);
			dectohex(atoi(current->name),hex);
			dectohex((hextodec(current->location)+hextodec(hex)-3),hex);
			strcpy(num,hex);//1039
		}
		else if(!strcmp(current->opcode,"RESW")){//opcode==RESW
			dectohex(hextodec(num)+3,hex);
			strcpy(current->location,hex);
			dectohex(atoi(current->name),hex);
			dectohex((hextodec(current->location)+atoi(current->name)*3-3),hex);
			strcpy(num,hex);
		}
		else if(!strcmp(current->opcode,"BYTE")){//opcode==BYTE
			dectohex(hextodec(num)+3,hex);
			strcpy(current->location,hex);
			if(current->name[0]=='C'){
				dectohex((hextodec(current->location)+strlen(current->name)-6),hex);
			}
			else{
				dectohex((hextodec(current->location)+(strlen(current->name)/2)-4),hex);
			}
			strcpy(num,hex);
		}
		else{
			dectohex(hextodec(num)+3,hex);
			strcpy(current->location,hex);
			strcpy(num,current->location);
		}
	}
	strcpy(current->location,"");
}
/************************************/
void symbol_table(){
	symbol=fopen("symbol.txt","w");
	fprintf(symbol,"%s	%s\n","Label Name","Address");
	source_node current=source_root;
	while(current){
		if(!strcmp(current->opcode,"START")){
			current=current->link;
			break;
		}
		current=current->link;
	} 
	while(current){
		if(strlen(current->labelname)!=0)
			fprintf(symbol,"%s		%s\n",current->labelname,current->location);
		current=current->link;
	}
}
/************************************/
void pass2_show(){
	pass2=fopen("pass2.txt","w");
	source_node current=source_root;
	while(current!=NULL){
		fprintf(pass2,"%s	",current->location);
		fprintf(pass2,"%s	   %s	%-20s	%s\n",current->labelname,current->opcode,current->name,current->object_code);
		current=current->link;
	}
}
/************************************/
void object(){
	source_node current=source_root;
	while(current){
		if(!strcmp(current->opcode,"START")){//find STRAT
			current=current->link;
			break;
		}
		current=current->link;
	}
	while(current){
		opnode ptr=opcode_root;
		while(ptr){
			if(!strcmp(ptr->name,current->opcode)){
				current->object_code[0]=ptr->number[0];
				current->object_code[1]=ptr->number[1];
				break;
			}
			ptr=ptr->link;
		}
		current=current->link;
	}
	current=source_root;
	source_node cur=source_root;
	while(current){
		if(!strcmp(current->opcode,"START")){
			strcpy(current->object_code,"");
			current=current->link;
			break;
		}
		current=current->link;
	}
	while(current->link){
		cur=source_root;
		if(strlen(current->name)==0){
			current=current->link;
			continue;
		}
		else if(!strcmp(current->opcode,"RESW")||!strcmp(current->opcode,"RESB")){//opcdode ==RESW||RESB reset
			reset(current->object_code);
		} 
		else if(current->name[strlen(current->name)-1]=='X'&&current->name[strlen(current->name)-2]==','){
			char hex[4],arr1[10];
			int j=5;
			strncpy(arr1,current->name,strlen(current->name)-2);
			while(cur){
				if(!strcmp(arr1,cur->labelname))
					break;
				cur=cur->link;
			}
			int n=hextodec(cur->location)+4096*8;//opcode + x + address(15bit)
			while(n>65536){
					n-=32768;
			}
			dectohex(n,hex);
			current->object_code[j--]=hex[3];
			current->object_code[j--]=hex[2];
			current->object_code[j--]=hex[1];
			current->object_code[j--]=hex[0];
		}
		else if((!strcmp(current->opcode,"BYTE"))&&current->name[0]=='X'){
			reset(current->object_code);
			int i,j=0;
			for(i=2;i<=strlen(current->name)-2;i++){
				current->object_code[j++]=current->name[i];
			}
		}
		else if((!strcmp(current->opcode,"BYTE"))&&current->name[0]=='C'){
			reset(current->object_code);
			int i,j=0,num;
			char hex[4];
			for(i=2;i<=strlen(current->name)-2;i++){
				dectohex(current->name[i],hex);
				current->object_code[j++]=hex[2];
				current->object_code[j++]=hex[3];
			}
		}
		else if(!strcmp(current->opcode,"WORD")){
			int j=5;
			char hex[4];
			dectohex(atoi(current->name),hex);
			current->object_code[j--]=hex[3];
			current->object_code[j--]=hex[2];
			current->object_code[j--]=hex[1];
			current->object_code[j--]=hex[0];
		}
		else{
			while(cur){
				if(!strcmp(cur->labelname,current->name)){
					current->object_code[2]=cur->location[0];
					current->object_code[3]=cur->location[1];
					current->object_code[4]=cur->location[2];
					current->object_code[5]=cur->location[3];
					break;
				}
				cur=cur->link;
			}
		}
		current=current->link;
	}
	strcpy(current->object_code,"");
}
/************************************/
void test_recoed(){
	record=fopen("record.txt","w");
	char name[6],length[4];
	int i,j=3,object_len=0;
	strcpy(length,"0000");
	strcpy(name,"      ");
	source_node current=source_root;
	while(current){
		if(!strcmp(current->opcode,"START")){
			for(i=0;i<strlen(current->labelname);i++)
				name[i]=current->labelname[i];
			for(i=strlen(current->name)-1;i>=0;i--)
				length[j--]=current->name[i];
			fprintf(record,"%c%s00%s",'H',name,length);
			strcpy(name,"000000");
			for(i=3;i>=0;i--)
				name[i+2]=length[i];
			break;
		}
		current=current->link;
	}
	if(!strcmp(current->link->opcode,"END"))//if second is END break
		return;
	while(strcmp(current->link->opcode,"END"))
		current=current->link;
	hextodec(current->location)-hextodec(length)+strlen(current->object_code)/2;//count total length
	dectohex(hextodec(current->location)-hextodec(length)+strlen(current->object_code)/2,length);
	fprintf(record,"00%s\n",length);
	current=source_root;
	while(current){
		if(!strcmp(current->opcode,"START")){
			current=current->link;
				break;		
			}
			current=current->link;
		}
	source_node ptr=current;
	while(1){//strcmp(current->link->object_code,"END")
		ptr=current;
		object_len=0;
		for(i=0;i<10;i++){//count number
			if(!strcmp(ptr->opcode,"RESW")||!strcmp(ptr->link->opcode,"END")){
				if(!strcmp(ptr->link->opcode,"END")){
					object_len+=strlen(ptr->object_code);
					j++;
				}
				else if(i--==0&&!strcmp(ptr->opcode,"RESW")){
					ptr=ptr->link;
					current=current->link;
					continue;
				}	
				break;
			}
			object_len+=strlen(ptr->object_code);//count object_sum
			if(strlen(ptr->object_code)==0){
				i--;
				current=current->link;
			}
			j=i;
			ptr=ptr->link;
		}
		fprintf(record,"%c00%s",'T',current->location);
		dectohex(object_len/2,length);//turn object_sum to hexdecimal
		fprintf(record,"%c%c",length[2],length[3]);
		for(i=0;i<=j;i++){//printf object_code
			fprintf(record,"%s",current->object_code);
			current=current->link;
		}
		if(!strcmp(current->opcode,"END"))//if EDN break
			break;
		fprintf(record,"\n");
	}
	strcpy(length,"0000");
	fprintf(record,"\n%c%s",'E',name);
}
/************************************/
int main(){
	FILE *fptr1,*fptr2;
	char name[10],number[2],c[10],arr[100],arr1[10],op[10];
	int i,cut,count,d=0;
	fptr1=fopen("opcode.txt","r");
	fptr2=fopen("source.txt","r");
	while(fscanf(fptr1,"%s%s",&name,&number)!=EOF)
		opcode_creat(name,number);
		
	while(fscanf(fptr2,"%[^\n] ",&arr)!=EOF){//cut source
		cut=0,count=0,d=0;
		reset(name);
		reset(op);
		reset(c);
		for(i=0;i<strlen(arr);i++){
			if(arr[i]=='	'){
				reset(arr1);
				strncpy(arr1,arr+cut,i-cut);
				if(check(arr1)){
					strcpy(op,arr1);
					d=1;//opcode
				}	
				else
					strcpy(name,arr1);
				cut=i+1;
				count++;
			}
			if(count==2||d==1){
				reset(arr1);
				strncpy(arr1,arr+cut,strlen(arr)-i);
				strcpy(c,arr1);
				break;
			}
		}
		if(count==0)
			strncpy(op,arr,strlen(arr));
		source_creat(name,op,c);
	}
	loc();//count location
	show_source();
	symbol_table();	
	object();
	pass2_show();
	test_recoed();
	fclose(fptr1);
	fclose(fptr2);
	fclose(symbol);
	fclose(pass1);
	fclose(pass2);
	fclose(record);
	return 0;
}
