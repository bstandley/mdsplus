#include <mdsobjects.h>
#include <mdsplus/mdsplus.h>
#include <mdsplus/AutoPointer.hpp>

#include <cstddef>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <semaphore.h>
#endif
using namespace MDSplus;
using namespace std;

extern "C"  void *getManyObj(char *serializedIn);
extern "C"  void *putManyObj(char *serializedIn);
extern "C" void *compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);
extern "C" int  SendArg(int sock, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims,
int *dims, char *bytes);
extern "C" int GetAnswerInfoTS(int sock, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr, void **m);
extern "C" int MdsOpen(int sock, char *tree, int shot);
extern "C" int MdsSetDefault(int sock, char *node);
extern "C" int MdsClose(int sock);
extern "C" int ConnectToMds(char *host);
extern "C" int ConnectToMdsEvents(char *host);
extern "C" int SetCompressionLevel(int level);
extern "C" int MdsSetCompression(int id, int level);
extern "C" void DisconnectFromMds(int sockId);
extern "C" void FreeMessage(void *m);


#define DTYPE_UCHAR_IP   2
#define DTYPE_USHORT_IP  3
#define DTYPE_ULONG_IP   4
#define DTYPE_ULONGLONG_IP 5
#define DTYPE_CHAR_IP    6
#define DTYPE_SHORT_IP   7
#define DTYPE_LONG_IP    8
#define DTYPE_LONGLONG_IP 9
#define DTYPE_FLOAT_IP   10
#define DTYPE_DOUBLE_IP  11
#define DTYPE_CSTRING_IP 14

static int convertType(int mdsType)
{
	switch(mdsType) {
		case DTYPE_B: return DTYPE_CHAR_IP;
		case DTYPE_BU: return DTYPE_UCHAR_IP;
		case DTYPE_W: return DTYPE_SHORT_IP;
		case DTYPE_WU: return DTYPE_USHORT_IP;
		case DTYPE_L: return DTYPE_LONG_IP;
		case DTYPE_LU: return DTYPE_ULONG_IP;
		case DTYPE_Q: return DTYPE_LONGLONG_IP;
		case DTYPE_QU: return DTYPE_ULONGLONG_IP;
		case DTYPE_FLOAT: return DTYPE_FLOAT_IP;
		case DTYPE_DOUBLE: return DTYPE_DOUBLE_IP;
		case DTYPE_T: return DTYPE_CSTRING_IP;
		default: return -1;
	}
}


void *getManyObj(char *serializedIn)
{
	AutoData<List> inArgs((List *)deserialize((const char *)serializedIn));
	if(inArgs->clazz != CLASS_APD)// || inArgs->dtype != DTYPE_LIST)
		throw MdsException("INTERNAL ERROR: Get Multi did not receive a LIST argument");

	int nArgs = inArgs->len();
	String nameKey("name");
	String exprKey("exp");
	String argsKey("args");
	AutoData<Dictionary> result(new Dictionary());
	for(int idx = 0; idx < nArgs; idx++)
	{
		AutoData<Dictionary> currArg((Dictionary *)inArgs->getElementAt(idx));
		if(currArg->clazz != CLASS_APD)// || currArg->dtype != DTYPE_DICTIONARY)
			throw MdsException("INTERNAL ERROR: Get Multi Argument is not a DICTIONARY argument");

		AutoData<String> nameData((String *)currArg->getItem(&nameKey));
		AutoData<String> exprData((String *)currArg->getItem(&exprKey));
		AutoArray<char> expr(exprData->getString());
		AutoData<List> argsData((List *)currArg->getItem(&argsKey));

		AutoData<Dictionary> answDict(new Dictionary());
		try {
			Data *currAnsw;
			if(argsData.get() && argsData->len() > 0)
				currAnsw = executeWithArgs(expr.get(), 2, argsData->getDscs(), argsData->len());
			else
				currAnsw = execute(expr.get());

			AutoData<String> valueKey(new String("value"));
			answDict->setItem(valueKey.get(), currAnsw);
		} catch(MdsException const & e) {
			AutoData<String> errorKey(new String("error"));
			AutoData<String> errorData(new String(e.what()));
			answDict->setItem(errorKey.get(), errorData.get());
		}
		result->setItem(nameData.get(), answDict.get());
	}

	return result->convertToDsc();
}

void *putManyObj(char *serializedIn)
{
	AutoData<List> inArgs((List *)deserialize((const char *)serializedIn));
	if(inArgs->clazz != CLASS_APD)// || inArgs->dtype != DTYPE_LIST)
		throw MdsException("INTERNAL ERROR: Get Multi did not receive a LIST argument");

	int nArgs = inArgs->len();
	String nodeKey("node");
	String exprKey("exp");
	String argsKey("args");
	AutoData<Dictionary> result(new Dictionary());
	for(int idx = 0; idx < nArgs; idx++) {
		AutoData<Dictionary> currArg((Dictionary *)inArgs->getElementAt(idx));
		if(currArg->clazz != CLASS_APD)// || currArg->dtype != DTYPE_DICTIONARY)
			throw MdsException("INTERNAL ERROR: Get Multi Argument is not a DICTIONARY argument");

		AutoData<String> nodeNameData((String *)currArg->getItem(&nodeKey));
		AutoData<String> exprData((String *)currArg->getItem(&exprKey));
		AutoArray<char> expr(exprData->getString());
		AutoData<List> argsData((List *)currArg->getItem(&argsKey));

		int nPutArgs = 0;
		if(argsData.get())
			nPutArgs = argsData->len();

		try {
			AutoPointer<Tree> tree(getActiveTree());
			AutoData<Data> compiledData = (Data *)compileFromExprWithArgs(expr.get(), nPutArgs, (argsData.get())?argsData->getDscs():0, tree.get());
			AutoPointer<TreeNode> node = tree->getNode(nodeNameData.get());
			node->putData(compiledData.get());
			AutoData<String> successData(new String("Success"));
			result->setItem(nodeNameData.get(), successData.get());
		} catch(MdsException const & e) {
			AutoData<String> errorData(new String(e.what()));
			result->setItem(nodeNameData.get(), errorData.get());
		}
	}

	return result->convertToDsc();
}

Mutex Connection::globalMutex;

Connection::Connection(char *mdsipAddr, int clevel) //mdsipAddr of the form <IP addr>[:<port>]
{
    lockGlobal();
    SetCompressionLevel(clevel);
    sockId = ConnectToMds(mdsipAddr);
    unlockGlobal();
	if(sockId <= 0) {
		std::string msg("Cannot connect to ");
		msg += mdsipAddr;
		throw MdsException(msg.c_str());
	}
}

Connection::~Connection() {
    lockGlobal();
	DisconnectFromMds(sockId);
    unlockGlobal();
}

void Connection::lockLocal() {
	mutex.lock();
}

void Connection::unlockLocal() {
	mutex.unlock();
}

void Connection::lockGlobal() {
	globalMutex.lock();
}

void Connection::unlockGlobal() {
	globalMutex.unlock();
}

void Connection::openTree(char *tree, int shot) {
	int status = MdsOpen(sockId, tree, shot);
	std::cout << "SOCK ID: " << sockId << std::endl;
	if(!(status & 1))
		throw MdsException(status);
}

void Connection::closeAllTrees() {
	int status = MdsClose(sockId);
	if(!(status & 1))
		throw MdsException(status);
}

Data *Connection::get(const char *expr, Data **args, int nArgs)
{
	char clazz, dtype, nDims;
	short length;
	int *dims;
	int status, numBytes;
	void *ptr, *mem = 0;
	char *buf;
	int retDims[MAX_DIMS];
	Data *resData;

	//Check whether arguments are compatible (Scalars or Arrays)
	for(std::size_t argIdx = 0; argIdx < nArgs; ++argIdx) {
		args[argIdx]->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
		delete [] dims;
		if(!ptr)
			throw MdsException("Invalid argument passed to Connection::get(). Can only be Scalar or Array");
	}

	lockLocal();
    //	lockGlobal();
	status = SendArg(sockId, 0, DTYPE_CSTRING_IP, nArgs+1, std::string(expr).size(), 0, 0, (char *)expr);
	if(!(status & 1)) {
		unlockLocal();
		throw MdsException(status);
	}

	for(std::size_t argIdx = 0; argIdx < nArgs; ++argIdx) {
		args[argIdx]->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
		status = SendArg(sockId, argIdx + 1, convertType(dtype), nArgs+1, length, nDims, dims, (char *)ptr);
		delete [] dims;
		if(!(status & 1))
		{
			unlockLocal();
			throw MdsException(status);
		}
	}
    //	unlockGlobal();	
    status = GetAnswerInfoTS(sockId, &dtype, &length, &nDims, retDims, &numBytes, &ptr, &mem);
	unlockLocal();
	if(!(status & 1))
		throw MdsException(status);

	if(nDims == 0) {
		switch(dtype) {
			case DTYPE_CHAR_IP:
				resData = new Int8(*(char *)ptr);
				break;
			case DTYPE_UCHAR_IP:
				resData = new Uint8(*(unsigned char *)ptr);
				break;
			case DTYPE_SHORT_IP:
				resData = new Int16(*(short *)ptr);
				break;
			case DTYPE_USHORT_IP:
				resData = new Uint16(*(unsigned short *)ptr);
				break;
			case DTYPE_LONG_IP:
				resData = new Int32(*(int *)ptr);
				break;
			case DTYPE_ULONG_IP:
				resData = new Uint32(*(unsigned int *)ptr);
				break;
			case DTYPE_LONGLONG_IP:
				resData = new Int64(*(long *)ptr);
				break;
			case DTYPE_ULONGLONG_IP:
				resData = new Uint64(*(unsigned long *)ptr);
				break;
			case DTYPE_FLOAT_IP:
				resData = new Float32(*(float *)ptr);
				break;
			case DTYPE_DOUBLE_IP:
				resData = new Float64(*(double *)ptr);
				break;
			case DTYPE_CSTRING_IP: {
				std::string buf(reinterpret_cast<char *>(ptr), length);
				resData = new String(buf.c_str());
				break;
			}
			default: 
				std::cerr << "Unexpected data type returned by mdsip: " << dtype << std::endl;
				throw MdsException("Unexpected data type returned by mdsip");
		}
	} else {
		//nDims > 0
		switch(dtype) {
			case DTYPE_CHAR_IP:
				resData = new Int8Array((char *)ptr, nDims, retDims);
				break;
			case DTYPE_UCHAR_IP:
				resData = new Uint8Array((unsigned char *)ptr, nDims, retDims);
				break;
			case DTYPE_SHORT_IP:
				resData = new Int16Array((short *)ptr, nDims, retDims);
				break;
			case DTYPE_LONG_IP:
				resData = new Int32Array((int *)ptr, nDims, retDims);
				break;
			case DTYPE_ULONG_IP:
				resData = new Uint32Array((unsigned int *)ptr, nDims, retDims);
				break;
			case DTYPE_LONGLONG_IP:
				resData = new Int64Array((int64_t *)ptr, nDims, retDims);
				break;
			case DTYPE_ULONGLONG_IP:
				resData = new Uint64Array((uint64_t *)ptr, nDims, retDims);
				break;
			case DTYPE_FLOAT_IP:
				resData = new Float32Array((float *)ptr, nDims, retDims);
				break;
			case DTYPE_DOUBLE_IP:
				resData = new Float64Array((double *)ptr, nDims, retDims);
				break;
			default:
				std::cerr << "Unexpected data type returned by mdsip: " << dtype << std::endl;
				throw MdsException("Unexpected data type returned by mdsip");
		}
	}

	if(mem) FreeMessage(mem);
	return resData;	
}

void Connection::put(const char *inPath, char *expr, Data **args, int nArgs)
{
	char clazz, dtype, nDims;
	short length;
	int *dims;
	int status;
	void *ptr, *mem = 0;

	//Check whether arguments are compatible (Scalars or Arrays)
	for(std::size_t argIdx = 0; argIdx < nArgs; ++argIdx) {
		args[argIdx]->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
		if(!ptr)
			throw MdsException("Invalid argument passed to Connection::put(). Can only be Scalar or Array");
	}

	//Double backslashes!!
	std::string path(inPath);
	if (path.at(0) == '\\')
		path.insert(path.begin(), '\\');

	std::string putExpr("TreePut(\'");
	putExpr += path + "\',\'" + expr + "\'";
	for(int varIdx = 0; varIdx < nArgs; ++varIdx)
		putExpr += ",$";
	putExpr += ")";

	lockLocal();
	status = SendArg(sockId, 0, DTYPE_CSTRING_IP, nArgs+1, putExpr.length(), 0, 0, const_cast<char *>(putExpr.c_str()));

	if(!(status & 1)) {
		unlockLocal();
		throw MdsException(status);
	}

	for(std::size_t argIdx = 0; argIdx < nArgs; ++argIdx) {
		args[argIdx]->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
		status = SendArg(sockId, argIdx + 1, convertType(dtype), nArgs+1, length, nDims, dims, (char *)ptr);
		if(!(status & 1)) {
			unlockLocal();
			throw MdsException(status);
		}
	}

	int retDims[MAX_DIMS];
	int numBytes;
    status = GetAnswerInfoTS(sockId, &dtype, &length, &nDims, retDims, &numBytes, &ptr, &mem);
	unlockLocal();
    if ((status & 1) && dtype == DTYPE_LONG_IP && nDims == 0 && numBytes == sizeof(int))
    	status = *(reinterpret_cast<int *>(ptr));
    if (mem) FreeMessage(mem);
	if(!(status & 1))
		throw MdsException(status);
}

void Connection::setDefault(char *path)
{
	int status = MdsSetDefault(sockId, path);
	if(!(status & 1))
		throw MdsException(status);
}

void GetMany::insert(int idx, char *name, char *expr, Data **args, int nArgs)
{
//They are freed by the List descructor
/*	AutoData<String> fieldName(new String("name"));
	AutoData<String> nameStr(new String(name));
	AutoData<String> fieldExp(new String("exp"));
	AutoData<String> exprStr(new String(expr));
*/
	String *fieldName = new String("name");
	String *nameStr = new String(name);
	String *fieldExp = new String("exp");
	String *exprStr = new String(expr);

	Dictionary *dict = new Dictionary();
	dict->setItem(fieldName, nameStr);
	dict->setItem(fieldExp, exprStr);

	String *argStr = new String("args");
	List *list = new List();
	for(int i = 0; i < nArgs; i++)
		list->append(args[i]);
	dict->setItem(argStr, list);

	if(idx >= len())
		List::append(dict);
	else
		List::insert(idx, dict);
}

void GetMany::append(char *name, char *expr, Data **args, int nArgs) {
	insert(len(), name, expr, args, nArgs);
}

void GetMany::insert(char * beforeName, char *name, char *expr, Data **args, int nArgs)
{
	int nItems = len();
	int idx;
	String nameKey("name");
	String nameStr(name);
	for(idx = 0; idx < nItems; idx++) {
		AutoData<Dictionary> currDict((Dictionary *)getElementAt(idx));
		AutoData<String> currName((String *)currDict->getItem(&nameKey));
		if(currName->equals(&nameStr))
			break;
	}

	insert(idx, name, expr, args, nArgs);
}

void GetMany::remove(char *name)
{
	int nItems = len();
	int idx;
	String nameKey("name");
	String nameStr(name);
	for(idx = 0; idx < nItems; idx++) {
		AutoData<Dictionary> currDict((Dictionary *)getElementAt(idx));
		AutoData<String> currName((String *)currDict->getItem(&nameKey));
		if(currName->equals(&nameStr))
			List::remove(idx);
	}
}

void GetMany::execute()
{
	int serSize;
	AutoArray<char> ser(serialize(&serSize));

	AutoData<Uint8Array> serData(new Uint8Array((unsigned char *)ser.get(), serSize));
	Data * serPtr = (Data *)serData.get();
	AutoData<Data> serEvalRes(conn->get("GetManyExecute($)", &serPtr, 1));
	evalRes = (Dictionary *)deserialize(serEvalRes.get());
}

Data *GetMany::get(char *name) {
	if(!evalRes)
		throw MdsException("Data not yet evaluated");

	String nameStr(name);
	AutoData<Dictionary> dictionary((Dictionary *)evalRes->getItem(&nameStr));
	if(!dictionary.get())
		throw MdsException("Missing data item in evaluation list");

	String valueKey("value");
	AutoData<Data> result(dictionary->getItem(&valueKey));
	if(result.get())
		return result.get();

	String errorKey("error");
	AutoData<String> error((String *)dictionary->getItem(&errorKey));
	if(!error.get())
		throw MdsException("Unknown Error");
	else {
		AutoPointer<char> errBuf(error->getString());
		throw MdsException(errBuf.get());
	}

	return 0;
}

/////////////////

void PutMany::insert(int idx, char *nodeName, char *expr, Data **args, int nArgs) {
	AutoData<String> node(new String("node"));
	AutoData<String> nodeNameStr(new String(nodeName));
	AutoData<String> exp(new String("exp"));
	AutoData<String> exprStr(new String(expr));

	AutoData<Dictionary> dict(new Dictionary());
	dict->setItem(node.get(), nodeNameStr.get());
	dict->setItem(exp.get(), exprStr.get());

	AutoData<String> argStr(new String("args"));
	AutoData<List> list(new List());
	for(int i = 0; i < nArgs; i++)
		list->append(args[i]);
	dict->setItem(argStr.get(), list.get());

	if(idx >= len())
		List::append(dict.get());
	else
		List::insert(idx, dict.get());
}

void PutMany::append(char *name, char *expr, Data **args, int nArgs) {
	insert(len(), name, expr, args, nArgs);
}

void PutMany::insert(char * beforeName, char *nodeName, char *expr, Data **args, int nArgs) {
	int nItems = len();
	int idx;
	String nameKey("node");
	String nodeNameStr(nodeName);
	for(idx = 0; idx < nItems; idx++) {
		AutoData<Dictionary> currDict((Dictionary *)getElementAt(idx));
		AutoData<String> currName = (String *)currDict->getItem(&nameKey);
		if(currName->equals(&nodeNameStr))
			break;
	}

	insert(idx, nodeName, expr, args, nArgs);
}

void PutMany::remove(char *nodeName) {
	std::size_t nItems = len();
	String nodeKey("node");
	String nodeNameStr(nodeName);
	for(std::size_t idx = 0; idx < nItems; ++idx) {
		AutoData<Dictionary> currDict = (Dictionary *)getElementAt(idx);
		AutoData<String> currName = (String *)currDict->getItem(&nodeKey);
		if(currName->equals(&nodeNameStr))
			List::remove(idx);
	}
}

void PutMany::execute() {
	int serSize;
	AutoArray<char> ser(serialize(&serSize));

	AutoData<Uint8Array> serData(new Uint8Array((unsigned char *)ser.get(), serSize));
	Data * serPtr = (Data *)serData.get();
	AutoData<Data> serEvalRes(conn->get("PutManyExecute($)", &serPtr, 1));
	evalRes = (Dictionary *)deserialize(serEvalRes.get());
}

void PutMany::checkStatus(char *nodeName) {
	if(!evalRes)
		throw MdsException("Data not yet written");

	String nodeNameStr(nodeName);
	AutoData<String> resItem((String *)evalRes->getItem(&nodeNameStr));
	if(!resItem.get())
		throw MdsException("Missing data item in evaluation list");

	if(!String("Success").equals(resItem.get())) {
		AutoArray<char> errMsg(resItem->getString());
		throw MdsException(errMsg.get());
	}
}
