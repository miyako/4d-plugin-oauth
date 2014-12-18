4d-plugin-oauth
===============

Collection of functions to help implement OAuth in the 4D language.

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
