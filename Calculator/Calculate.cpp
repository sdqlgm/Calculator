#include "Calculate.h"
#include<stack>

//操作符的数目
#define	OPTR_NUM	15

//操作符在OPSET中的位置
#define	OP_INDEX_ADD	0
#define	OP_INDEX_MINUS	1
#define	OP_INDEX_X	2
#define	OP_INDEX_DIV	3
#define	OP_INDEX_POW	4
#define	OP_INDEX_LN	5
#define	OP_INDEX_LOG	6
#define	OP_INDEX_SIN	7
#define	OP_INDEX_COS	8
#define	OP_INDEX_TAN	9
#define	OP_INDEX_SQR	10
#define	OP_INDEX_SQRT	11
#define	OP_INDEX_LEFT	12
#define	OP_INDEX_RIGHT	13
#define	OP_INDEX_EQUAL	14

TCHAR *OPSET[OPTR_NUM] = { L"+",L"-",L"×",L"÷",L"^",L"ln",L"log",L"sin",L"cos",L"tan",L"sqr",	L"√",	L"(",	L")",	L"=" };//操作符集

TCHAR Prior[OPTR_NUM][OPTR_NUM] = { 
	/* 算符间的优先关系*/
	//'+','-','×','÷','^','ln','log','sin','cos','tan','sqr','√','(',')','='
	  '>','>','<','<', '<','<',  '<',  '<',  '<',  '<',   '<', '<',  '<','>','>',//'+'
	  '>','>','<','<', '<','<',  '<',  '<',  '<',  '<',   '<', '<',  '<','>','>',//,'-'
	  '>','>','>','>', '<','<',  '<',  '<',  '<',  '<',   '<', '<',  '<','>','>',//'×'
	  '>','>','>','>', '<','<',  '<',  '<',  '<',  '<',   '<', '<',  '<','>','>',//'÷'
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  '<','>','>',//'^'
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  '<','>','>',//'ln'
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  '<','>','>',//'log'
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  '<','>','>',//'sin'
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  '<','>','>',//'cos'
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  '<','>','>',//'tan'
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  '<','>','>',//'sqr'
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  '<','>','>',//'√'
	  '<','<','<','<', '<','<',  '<',  '<',  '<',  '<',   '<', '<',  '<','=',' ',//'('
	  '>','>','>','>', '>','>',  '>',  '>',  '>',  '>',   '>', '>',  ' ','>','>',//')'
	  '<','<','<','<', '<','<',  '<',  '<',  '<',  '<',   '<', '<',  '<',' ','='//'='
};
TCHAR Precede(int a, int b) {
	/*返回操作符的优先关系，例如 + < *  */
	return Prior[a][b];//例如+和*对应的位置是Prior[1][3]
}
int is_num(TCHAR s){
	/*判断s是不是操作数*/
	switch (s)
	{
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
	case '.':
		return 1;
	default:
		return 0;
	}
	return 0;
}
int get_operator_index(const TCHAR *s){
	/*判断s是不是操作符*/
	for (int i = 0; i< OPTR_NUM; i++) {
		if (wcscmp(s, OPSET[i]) == 0) return i;
	}
	return 0;
}
BOOL Operate(double a,int theta, double b, double &result, int &error_code) {
	//运算 a theta b，例如 a*b，然后返回计算后的值
	//单操作数只使用b
	/*L"+",	L"-",	L"×",	L"÷",	L"ln",	L"log",	L"sin",	L"cos",	L"tan",	L"％",	L"sqr",	L"√",	L"(",	L")",	L"="*/
	switch (theta) {
	case OP_INDEX_ADD:
		result= a + b;
		break;
	case OP_INDEX_MINUS:
		result = a - b;
		break;
	case OP_INDEX_X:
		result = a * b;
		break;
	case OP_INDEX_DIV:
		if (b == 0) {
			//除数不能为0
			error_code = ERROR_CODE_NOZERO;
			result = 0;
			return FALSE;
		}
		result = a / b;
		break;
	case OP_INDEX_LN:
		if (b <= 0) {
			//错误的输入
			error_code = ERROR_CODE_WORRYNUM;
			result = 0;
			return FALSE;
		}
		result = log(b);
		break;
	case OP_INDEX_LOG:
		if (b <= 0) {
			//错误的输入
			error_code = ERROR_CODE_WORRYNUM;
			result = 0;
			return FALSE;
		}
		result = log10(b);
		break;
	case OP_INDEX_SIN:
		result = sin(b);
		break;
	case OP_INDEX_COS:
		result = cos(b); 
		break;
	case OP_INDEX_TAN:
		result = tan(b);
		break;
	case OP_INDEX_POW:
		result = pow(a, b);
		break;
	case OP_INDEX_SQR:
		result = b*b;
		break;
	case OP_INDEX_SQRT:
		if (b < 0) {
			//错误的输入
			error_code = ERROR_CODE_WORRYNUM;
			result = 0;
			return FALSE;
		}
		result = sqrt(b);
		break;
	//case OP_INDEX_LEFT:
	//case OP_INDEX_RIGHT:
	//case OP_INDEX_EQUAL:
	}
	return TRUE;
}
double get_next_expression(const TCHAR * Expression,int &index,int &isnum){
	/*从表达式Expression中读入一个新的数字或者操作符*
	/*例如12+sin(5)=，依次读入12、+、sin、*/
	/*如果是操作符，则返回操作符在OPSET中的位置*/
	/*index表示从哪里开始读，isnum表示读入的是数字还是操作符*/
	TCHAR c;
	TCHAR str[20];
	double ret;


	memset(str, 0, sizeof(str));

	//先判断将要读入的是操作符还是数字
	isnum = is_num(Expression[index]);
	while (index < wcslen(Expression)) {
		c = Expression[index]; //先预读一个
		//一个数字取出完成
		if (is_num(c) != isnum)return _wtof(str);
		index++;

		if (isnum) {
			str[wcslen(str)] = c;//将c连接到str
		}
		else {
			str[wcslen(str)] = c;//连接c到str
			if (ret=get_operator_index(str)) {
				//判断str是不是一个完整的操作符
				//一个操作符取出完成
				return ret;
			}
		}
	
	}
	return 0;
}

BOOL EvaluateExpression(const TCHAR * Expression,double &result,int &error_code){
	/*计算表达式Expression,如2(1+2)= */
	/*结果在result中，失败日志在error_code中*/
	int index = 0;
	double a=0, b=0;		//两个操作数
	double op;			//操作数在OPSET中的位置，或者是某个操作数
	int theta;			//从栈中取出的操作符
	std::stack<double> OPND;	//操作数栈,存储操作数
	std::stack<double> OPTR;	//操作符栈,存储操作符在OPSET中的位置
	int isnum;
	int lastType = 0;		//上次取出的字符的种类，也就是上次记录的isnum

	OPTR.push(OP_INDEX_EQUAL);
	op = get_next_expression(Expression, index, isnum);
	while (op!= OP_INDEX_EQUAL || OPTR.top() != OP_INDEX_EQUAL){
		if (isnum){
			//操作数
			OPND.push(op);
			lastType = isnum;
			op = get_next_expression(Expression, index, isnum);
		}else{
			//操作符
			switch (Precede(OPTR.top(), op)){
			case '<'://操作符入栈 
				/*用户忘了输入乘号,补一个乘号*/
				//有几种情况：2(   2sin   )sin
				if ((lastType == 1 && isnum == 0) && (op >= 5 && op <= 12)) {
					//也就是 2( 2sin 3cos 这些情况
					//入栈一个乘号
					OPTR.push(OP_INDEX_X);
				}
				/*将当前操作符入栈*/
				OPTR.push(op);
				lastType = isnum;
				op = get_next_expression(Expression, index, isnum);
				break;
			case '='://脱括号
				OPTR.pop();
				lastType = isnum;
				op = get_next_expression(Expression, index, isnum);
				break;
			case '>'://退栈并计算
				theta = OPTR.top();;
				OPTR.pop();

				//只需要一个操作数的操作符只出栈一个操作数
				if (theta >= 5 && theta <= 11) {
					//从第4个到11个操作符，即log~sqrt
					if (OPND.size() <= 0) {
						//表达式错误
						error_code = ERROR_CODE_WORRYEXP;
						return FALSE;
					}
					b = OPND.top();
					OPND.pop();
				}
				else {
					if (OPND.size() <= 1) {
						//表达式错误
						error_code = ERROR_CODE_WORRYEXP;
						return FALSE;
					}
					b = OPND.top();
					OPND.pop();
					a = OPND.top();
					OPND.pop();
				}
				if (!Operate(a, theta, b, result, error_code)) {
					//计算错误
					return FALSE;
				}
				OPND.push(result);//将运算后的结果压入栈顶
				break;
			}
		}
	}
	//计算完了栈里还有数据，那就是错了
	if (OPND.size() >= 2)
	{
		error_code = ERROR_CODE_WORRYEXP;
		return FALSE;
	}
	result = OPND.top();//计算完成，返回结果
	return TRUE;
}