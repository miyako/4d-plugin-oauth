#include "functions_json.h"
#include "libjson/libjson.h"

void _copyString(C_TEXT *t, std::wstring *p){
#if VERSIONMAC	
	CUTF8String s;
	t->copyUTF8String(&s);
	
	iconv_t conv_desc = iconv_open(UTF32NATIVE, "UTF-8");
	
	if ((size_t)conv_desc != (size_t) -1){
		
		const char *inData = (const char *)s.c_str();		
		size_t inDataLen = (size_t)s.length();
		size_t outDataLen = (size_t)(t->getUTF16Length() * 4) + 4;		
		
		char *outData = (char *)calloc(outDataLen, 1);
		char *outDataPtr = outData;		
		
		size_t iconv_value;
		
		iconv_value = iconv (conv_desc, (char **)&inData, &inDataLen, &outData, &outDataLen);
		
		if (!iconv_value){
			*p = std::wstring((const wchar_t *)outDataPtr, wcslen((const wchar_t *)outDataPtr));				
		}
		
		free(outDataPtr);	
		iconv_close (conv_desc);		
	}	
#else	
	*p = std::wstring((wchar_t *)t->getUTF16StringPtr());
#endif	
}	

void _copyString(std::wstring *p, C_TEXT *t){
#if VERSIONMAC	
	iconv_t conv_desc = iconv_open("UTF-8", UTF32NATIVE);	
	
	if ((size_t)conv_desc != (size_t) -1){
		
		const char *inData = (const char *)p->c_str();		
		size_t inDataLen = (size_t)p->length() * sizeof(wchar_t);
		size_t outDataLen = (size_t)(p->length() * 4) + 1;			
		
		char *outData = (char *)calloc(outDataLen, 1);
		char *outDataPtr = outData;			
		
		size_t iconv_value;
		
		iconv_value = iconv (conv_desc, (char **)&inData, &inDataLen, &outData, &outDataLen);
		
		if (!iconv_value){
			t->setUTF8String((const uint8_t *)outDataPtr, strlen(outDataPtr));		
		}		
		
		free(outDataPtr);	
		iconv_close (conv_desc);	
	}	
#else
	t->setUTF16String((const PA_Unichar*)p->c_str(), (uint32_t)p->length());
#endif	
}

void _toHex(JSONNODE *n, C_TEXT *hex){
	
	CUTF8String u;
	size_t len = (sizeof(size_t)*2);
	std::vector<uint8_t> buf(len+3);//on mac, we get the 0x prefix
	
	sprintf((char *)&buf[0], "%p", n);
#if VERSIONMAC
	u.append(&buf[2], len);		
#else	
	u.append(&buf[0], len);	
#endif	
	hex->setUTF8String(&u);	
}

JSONNODE * _fromHex(C_TEXT *hex){
	
	void *p = NULL;
	CUTF8String u;
	hex->copyUTF8String(&u);
	
	sscanf((const char *)u.c_str(), "%p", &p);	
	
	return p;
}

void JSON_CLEAR(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n) json_clear(n);
}

void JSON_SET_BOOL(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT value;
	
	json.fromParamAtIndex(pParams, 1);
	value.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n) json_set_b(n, value.getIntValue());
}

void JSON_Append_node(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	C_TEXT nodeText;	
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	nodeText.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		JSONNODE *node;
		
		if(!nodeText.getUTF16Length()){
			node = json_new(JSON_NODE);
		}else{
			std::wstring w;
			_copyString(&nodeText, &w);
			node = json_parse(w.c_str());
		}
		
		if(node){
			json_set_name(node, nodeName.c_str());
			json_push_back(n, node);	
		}
		
		_toHex(node, &returnValue);		
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Append_bool(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	C_LONGINT value;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	value.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		JSONNODE *node = json_new_b(nodeName.c_str(), value.getIntValue());
		json_push_back(n, node);
		_toHex(node, &returnValue);		
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Append_real(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	C_REAL value;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	value.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		JSONNODE *node = json_new_f(nodeName.c_str(), value.getDoubleValue());
		json_push_back(n, node);
		_toHex(node, &returnValue);		
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Append_long(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	C_LONGINT value;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	value.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		JSONNODE *node = json_new_i(nodeName.c_str(), value.getIntValue());
		json_push_back(n, node);
		_toHex(node, &returnValue);		
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Append_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	C_TEXT value;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	value.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		std::wstring valueText;
		_copyString(&value, &valueText);	
		
		JSONNODE *node = json_new_a(nodeName.c_str(), valueText.c_str());
		json_push_back(n, node);
		_toHex(node, &returnValue);
	}	
	
	returnValue.setReturn(pResult);
}

void JSON_Append_text_array(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	ARRAY_TEXT values;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	values.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		JSONNODE *node = json_new(JSON_ARRAY);
		json_set_name(node, nodeName.c_str());
		
		for (unsigned int i = 1; i < values.getSize(); ++i) {
			CUTF16String s;
			values.copyUTF16StringAtIndex(&s, i);
			C_TEXT t;
			t.setUTF16String(&s);
			std::wstring valueText;
			_copyString(&t, &valueText);
			json_push_back(node, json_new_a(L"", valueText.c_str()));			
		}
		
		json_push_back(n, node);
		_toHex(node, &returnValue);
	}	
	
	returnValue.setReturn(pResult);
}

void JSON_Append_long_array(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	ARRAY_LONGINT values;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	values.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		JSONNODE *node = json_new(JSON_ARRAY);
		json_set_name(node, nodeName.c_str());
		
		for (unsigned int i = 1; i < values.getSize(); ++i) {
			json_push_back(node, json_new_i(L"", values.getIntValueAtIndex(i)));			
		}
		
		json_push_back(n, node);
		_toHex(node, &returnValue);
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Append_real_array(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	ARRAY_REAL values;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	values.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		JSONNODE *node = json_new(JSON_ARRAY);
		json_set_name(node, nodeName.c_str());
		
		for (unsigned int i = 1; i < values.getSize(); ++i) {
			json_push_back(node, json_new_f(L"", values.getDoubleValueAtIndex(i)));			
		}
		
		json_push_back(n, node);
		_toHex(node, &returnValue);
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Append_bool_array(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	ARRAY_BOOLEAN values;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	values.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);
		
		JSONNODE *node = json_new(JSON_ARRAY);
		json_set_name(node, nodeName.c_str());
		
		for (unsigned int i = 1; i < values.getSize(); ++i) {
			json_push_back(node, json_new_b(L"", values.getBooleanValueAtIndex(i)));			
		}
		
		json_push_back(n, node);
		_toHex(node, &returnValue);
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Append_array(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT node;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	node.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring nodeName;
		_copyString(&node, &nodeName);

		JSONNODE *node = json_new(JSON_NODE);
		
		json_set_name(node, nodeName.c_str());
		json_cast(node, JSON_ARRAY);
		json_push_back(n, node);	
		
		_toHex(node, &returnValue);		
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Append_array_element(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT source;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	source.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
	
		if(json_type(n) == JSON_ARRAY)
		{
			JSONNODE *node = _fromHex(&source);
			
			if(node){
				JSONNODE *nodeCopy = json_duplicate(node);
				json_push_back(n, nodeCopy);
				_toHex(nodeCopy, &returnValue);
			}
			
		}

	}
	
	returnValue.setReturn(pResult);
}

void JSON_SET_REAL(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_REAL value;
	
	json.fromParamAtIndex(pParams, 1);
	value.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n) json_set_f(n, value.getDoubleValue());
}

void JSON_SET_LONG(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT value;
	
	json.fromParamAtIndex(pParams, 1);
	value.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n) json_set_i(n, value.getIntValue());
}

void JSON_SET_TEXT(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT value;
	
	json.fromParamAtIndex(pParams, 1);
	value.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring w;
		_copyString(&value, &w);
		json_set_a(n, w.c_str());	
	}
}

void JSON_SET_NAME(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT name;
	
	json.fromParamAtIndex(pParams, 1);
	name.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring w;
		_copyString(&name, &w);
		json_set_name(n, w.c_str());	
	}
}

void JSON_SET_COMMENT(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT comment;
	
	json.fromParamAtIndex(pParams, 1);
	comment.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring w;
		_copyString(&comment, &w);
		json_set_comment(n, w.c_str());	
	}
}

void JSON_SET_NULL(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n) json_nullify(n);
}

void JSON_SET_TYPE(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT type;
	
	json.fromParamAtIndex(pParams, 1);
	type.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){		
		switch (type.getIntValue()) {
			case 0:
				json_cast(n, JSON_NULL);
				break;
			case 1:
				json_cast(n, JSON_STRING);
				break;
			case 2:
				json_cast(n, JSON_NUMBER);
				break;
			case 3:
				json_cast(n, JSON_BOOL);
				break;			
			case 4:
				json_cast(n, JSON_ARRAY);
				break;		
			case 5:
				json_cast(n, JSON_NODE);
				break;					
			default:
				break;
		}	
	}
}

// ------------------------------------- Read -------------------------------------

void JSON_DELETE_ITEM_BY_POSITION(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT pos;
	
	json.fromParamAtIndex(pParams, 1);
	pos.fromParamAtIndex(pParams, 2);
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		JSONNODE_ITERATOR i = json_begin(n);
		int p = 1; 
		while (i != json_end(n)){
			if(p == pos.getIntValue()){
				json_erase(n, i);
				break;
			}
			++i;	
		}	
	}
}

void JSON_DELETE_ITEM_BY_NAME(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT name;
	C_LONGINT option;
	
	json.fromParamAtIndex(pParams, 1);
	name.fromParamAtIndex(pParams, 2);
	option.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring w;
		_copyString(&name, &w);
		
		JSONNODE_ITERATOR i;
		
		switch (option.getIntValue()) {
			case 1:
				i = json_find_nocase(n, w.c_str());
				break;
			default:
				i = json_find(n, w.c_str());
				break;
		}
		/*
		while (i != json_end(n)){
			json_erase(n, i);
			++i;	
		}
		 */
		if (i != json_end(n)){
			json_erase(n, i);	
		}
	}
}

void JSON_GET_REAL_ARRAY(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	ARRAY_REAL values;
	
	json.fromParamAtIndex(pParams, 1);
	values.setSize(1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		if(json_type(n) == JSON_ARRAY){
			JSONNODE_ITERATOR i = json_begin(n);
			while (i != json_end(n)){
				if (*i){
					values.appendDoubleValue(json_as_float(*i));					
				}
				++i;
			}
		}	
	}
	
	values.toParamAtIndex(pParams, 2);
}

void JSON_GET_TEXT_ARRAY(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	ARRAY_TEXT values;
	
	json.fromParamAtIndex(pParams, 1);
	values.setSize(1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		if(json_type(n) == JSON_ARRAY){
			JSONNODE_ITERATOR i = json_begin(n);
			while (i != json_end(n)){
				if (*i){
					std::wstring w = json_as_string(*i);
					C_TEXT t;
					_copyString(&w, &t);
					CUTF16String u;
					t.copyUTF16String(&u);
					values.appendUTF16String(&u);					
				}
				++i;
			}
		}	
	}
	
	values.toParamAtIndex(pParams, 2);
}

void JSON_GET_LONG_ARRAY(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	ARRAY_LONGINT values;
	
	json.fromParamAtIndex(pParams, 1);
	values.setSize(1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		if(json_type(n) == JSON_ARRAY){
			JSONNODE_ITERATOR i = json_begin(n);
			while (i != json_end(n)){
				if (*i){
					values.appendIntValue(json_as_int(*i));					
				}
				++i;
			}
		}	
	}
	
	values.toParamAtIndex(pParams, 2);
}

void JSON_GET_BOOL_ARRAY(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	ARRAY_BOOLEAN values;
	
	json.fromParamAtIndex(pParams, 1);
	values.setSize(1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		if(json_type(n) == JSON_ARRAY){
			JSONNODE_ITERATOR i = json_begin(n);
			while (i != json_end(n)){
				if (*i){
					values.appendBooleanValue(json_as_bool(*i));					
				}
				++i;
			}
		}	
	}
	
	values.toParamAtIndex(pParams, 2);
}

void JSON_Get_comment(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring w = json_get_comment(n);
		_copyString(&w, &returnValue);	
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Get_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring w = json_as_string(n);
		_copyString(&w, &returnValue);	
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Get_name(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring w = json_name(n);
		_copyString(&w, &returnValue);	
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Get_type(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		switch (json_type(n)) {
			case JSON_NULL:
				returnValue.setIntValue(0);
				break;
			case JSON_STRING:
				returnValue.setIntValue(1);
				break;
			case JSON_NUMBER:
				returnValue.setIntValue(2);
				break;
			case JSON_BOOL:
				returnValue.setIntValue(3);
				break;
			case JSON_ARRAY:
				returnValue.setIntValue(4);
				break;	
			case JSON_NODE:
				returnValue.setIntValue(5);
				break;					
		}
	} 
	
	returnValue.setReturn(pResult);
}

void JSON_Get_long(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n) returnValue.setIntValue(json_as_int(n));
	
	returnValue.setReturn(pResult);
}

void JSON_Get_real(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_REAL returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n) returnValue.setDoubleValue(json_as_float(n));
	
	returnValue.setReturn(pResult);
}

void JSON_Get_bool(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n) returnValue.setIntValue(json_as_bool(n));
	
	returnValue.setReturn(pResult);
}

void JSON_Get_child_by_position(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT pos;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	pos.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		JSONNODE *node = json_at(n, pos.getIntValue()-1);
		_toHex(node, &returnValue);
	}
	
	returnValue.setReturn(pResult);
}

void JSON_Get_child_by_name(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT name;
	C_LONGINT option;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	name.fromParamAtIndex(pParams, 2);
	option.fromParamAtIndex(pParams, 3);
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		std::wstring w;
		_copyString(&name, &w);
		JSONNODE *node = NULL;
		switch (option.getIntValue()) {
			case 1:
				node = json_get_nocase(n, w.c_str());
				break;
			default:
				node = json_get(n, w.c_str());
				break;
		}
		
		_toHex(node, &returnValue);
	}
	
	returnValue.setReturn(pResult);
}

void JSON_GET_CHILD_NODES(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	ARRAY_TEXT nodes;
	ARRAY_LONGINT types;
	ARRAY_TEXT names;
	
	json.fromParamAtIndex(pParams, 1);
	nodes.setSize(1);
	types.setSize(1);
	names.setSize(1);	
	
	JSONNODE *n = _fromHex(&json);
	
	if(n){
		JSONNODE_ITERATOR i = json_begin(n);
		while (i != json_end(n)){
			if (*i){
				
				std::wstring w = json_name(*i);
				C_TEXT t;
				_copyString(&w, &t);
				CUTF16String nodeName;
				t.copyUTF16String(&nodeName);
				names.appendUTF16String(&nodeName);
				
				C_TEXT h;
				_toHex(*i, &h);
				CUTF16String nodeRef;
				h.copyUTF16String(&nodeRef);	
				nodes.appendUTF16String(&nodeRef);				
				
				switch (json_type(*i)) {
					case JSON_NULL:
						types.appendIntValue(0);
						break;
					case JSON_STRING:
						types.appendIntValue(1);					
						break;
					case JSON_NUMBER:
						types.appendIntValue(2);							
						break;
					case JSON_BOOL:
						types.appendIntValue(3);							
						break;	
					case JSON_ARRAY:
						types.appendIntValue(4);							
						break;
					case JSON_NODE:
						types.appendIntValue(5);							
						break;							
				}
			}
			++i;			
		}
	}
	
	nodes.toParamAtIndex(pParams, 2);
	types.toParamAtIndex(pParams, 3);
	names.toParamAtIndex(pParams, 4);
}

// ------------------------------------- JSON -------------------------------------


void JSON_New(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;
	
	JSONNODE *n = json_new(JSON_NODE);
	
	_addJsonRootToList(n);
	
	_toHex(n, &returnValue);
	
	returnValue.setReturn(pResult);
}

void JSON_CLOSE(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	
	json.fromParamAtIndex(pParams, 1);
	
	JSONNODE *n = _fromHex(&json);
	
	if(_removeJsonRootFromList(n)){
		json_delete(n);
	}
}

void JSON_Export_to_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_LONGINT option;	
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	option.fromParamAtIndex(pParams, 2);
	
	JSONNODE *n = _fromHex(&json);
	std::wstring w;
	
	if(n){
		switch (option.getIntValue()) {
			case 1:
				w = json_write(n);
				break;
			default:
				w = json_write_formatted(n);				
				break;
		}
		
		_copyString(&w, &returnValue);
	}	
	
	returnValue.setReturn(pResult);
}

void JSON_Parse_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT source;
	C_TEXT returnValue;
	
	source.fromParamAtIndex(pParams, 1);
	
	std::wstring w;
	_copyString(&source, &w);
	
	JSONNODE *n = json_parse(w.c_str());
	
	_addJsonRootToList(n);
	
	_toHex(n, &returnValue);
	
	returnValue.setReturn(pResult);
}

// ------------------------------------ Utility -----------------------------------


void JSON_Strip_white_space(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT json;
	C_TEXT returnValue;
	
	json.fromParamAtIndex(pParams, 1);
	
	std::wstring w;
	_copyString(&json, &w);
	
	std::wstring j = json_strip_white_space((const json_char *)w.c_str());
	
	_copyString(&j, &returnValue);
	
	returnValue.setReturn(pResult);
}
