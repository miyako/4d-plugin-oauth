4d-plugin-oauth
===============

Collection of functions to help implement OAuth in the 4D language.

***Deprecated***

Please use 

https://github.com/miyako/4d-plugin-zip

https://github.com/miyako/4d-plugin-curl

https://github.com/miyako/4d-plugin-common-crypto

https://github.com/miyako/4d-plugin-json

https://github.com/miyako/4d-plugin-split-pictures

[Generate digest](http://doc.4d.com/4Dv15/4D/15/Generate-digest.301-2006181.en.html)

[Select document](http://doc.4d.com/4Dv15/4D/15.1/Select-document.301-2685312.en.html)

##Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|🆗|🚫|🆗|🆗|

Commands
---

```c
RSASHA256
HMACMD5
HMACSHA1
HMACSHA256
HMACSHA384
HMACSHA512
SHA384
SHA512
MD5
SHA1
SHA256
PEM_From_P12
RSASHA1

cURL
cURL_Escape_url
cURL_Unescape_url
cURL_Get_version
cURL_Get_date

Unzip
Zip

JSON_Strip_white_space
JSON_Parse_text
JSON_Export_to_text
JSON_CLOSE
JSON_New
JSON_GET_BOOL_ARRAY
JSON_GET_CHILD_NODES
JSON_GET_LONG_ARRAY
JSON_Get_child_by_name
JSON_Get_child_by_position
JSON_Get_comment
JSON_GET_TEXT_ARRAY
JSON_GET_REAL_ARRAY
JSON_Get_bool
JSON_Get_real
JSON_Get_long
JSON_Get_type
JSON_Get_name
JSON_Get_text
JSON_DELETE_ITEM_BY_NAME
JSON_DELETE_ITEM_BY_POSITION
JSON_SET_TYPE
JSON_SET_NULL
JSON_SET_COMMENT
JSON_SET_NAME
JSON_SET_TEXT
JSON_SET_LONG
JSON_SET_REAL
JSON_Append_bool_array
JSON_Append_real_array
JSON_Append_long_array
JSON_Append_text_array
JSON_Append_text
JSON_Append_long
JSON_Append_real
JSON_Append_bool
JSON_Append_node
JSON_SET_BOOL
JSON_CLEAR
JSON_Append_array
JSON_Append_array_element

PATH_From_user_selection
PATH_Get_known_folder
VOLUME_Is_ejectable
VOLUME_Is_remote
STRUCTURE_Import_definition
SYSTEM_Get_unixtime
SYSTEM_Generate_UUID
SYSTEM_Get_timestring
SYSTEM_Get_timestamp
PROCESS_Get_id
PROCESS_GET_LIST
STACK_Get_available_size
PICTURE_GET_RAW_DATA
```

Example
-------
* FTP (download a file)
```
C_BLOB($in;$out)
C_LONGINT($err)
ARRAY LONGINT($tNomOption;0)
ARRAY TEXT($tValOption;0)

$err:=cURL ("ftp://ftp.4d.com/Favicon.ico";$tNomOption;$tValOption;$in;$out)

BLOB TO DOCUMENT(System folder(Desktop)+"Favicon.ico";$out)
```

* FTP (upload a file)
```
C_BLOB($in;$out)
C_LONGINT($err)
ARRAY LONGINT($tNomOption;0)
ARRAY TEXT($tValOption;0)
APPEND TO ARRAY($tNomOption;CURLOPT_USERNAME)
APPEND TO ARRAY($tValOption;"user")
APPEND TO ARRAY($tNomOption;CURLOPT_PASSWORD)
APPEND TO ARRAY($tValOption;"pass")
APPEND TO ARRAY($tNomOption;CURLOPT_UPLOAD)
APPEND TO ARRAY($tValOption;"1")

DOCUMENT TO BLOB(Structure file;$in)

$err:=cURL ("ftp://ftp.4d.com/test.4db";$tNomOption;$tValOption;$in;$out)
```

* SFTP

```
C_BLOB($in;$out)
C_LONGINT($err)
ARRAY LONGINT($tNomOption;0)
ARRAY TEXT($tValOption;0)
APPEND TO ARRAY($tNomOption;CURLOPT_USERNAME)
APPEND TO ARRAY($tValOption;"user")
APPEND TO ARRAY($tNomOption;CURLOPT_PASSWORD)
APPEND TO ARRAY($tValOption;"password")
APPEND TO ARRAY($tNomOption;CURLOPT_SSL_VERIFYHOST)
APPEND TO ARRAY($tValOption;"0")
APPEND TO ARRAY($tNomOption;CURLOPT_SSL_VERIFYPEER)
APPEND TO ARRAY($tValOption;"0")
APPEND TO ARRAY($tNomOption;CURLOPT_DEBUGFUNCTION)
APPEND TO ARRAY($tValOption;"CB_DEBUG")

$err:=cURL ("sftp://server.fr/";$tNomOption;$tValOption;$in;$out)
```
