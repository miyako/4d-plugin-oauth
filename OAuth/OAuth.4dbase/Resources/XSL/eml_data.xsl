<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:str="http://exslt.org/strings">
				
<xsl:output method="text" />

<xsl:param name="content_type" select="text/plain" />

<xsl:template match="/">

	<xsl:variable name="has_attachments" select="/eml/attachments/attachment" />

	<xsl:choose>
		<xsl:when test="$has_attachments">
		<xsl:text>Content-Type: multipart/mixed; boundary=</xsl:text><xsl:value-of select="/eml/attachments/@boundary" />
		<xsl:text>&#xD;&#xA;</xsl:text>	
		</xsl:when>
		<xsl:otherwise>
		<xsl:text>Content-Type: </xsl:text><xsl:value-of select="$content_type" /><xsl:text>; charset=utf-8</xsl:text>		
		<xsl:text>&#xD;&#xA;</xsl:text>	
		<xsl:text>Content-Transfer-Encoding: base64</xsl:text>		
		<xsl:text>&#xD;&#xA;</xsl:text>			
		</xsl:otherwise>
	</xsl:choose>
		
	<xsl:for-each select="/eml/headers/header">
	<xsl:sort select="./@name" />
		<xsl:value-of select="concat(./@name, ': ', ./@value)" />
		<xsl:text>&#xD;&#xA;</xsl:text>		
	</xsl:for-each>

		<xsl:text>&#xD;&#xA;</xsl:text>

	<xsl:choose>
	<xsl:when test="$has_attachments">
	
		<xsl:text>--</xsl:text><xsl:value-of select="/eml/attachments/@boundary" />
		<xsl:text>&#xD;&#xA;</xsl:text>
		<xsl:text>Content-Type: </xsl:text><xsl:value-of select="$content_type" /><xsl:text>; charset=utf-8</xsl:text>		
		<xsl:text>&#xD;&#xA;</xsl:text>	
		<xsl:text>Content-Transfer-Encoding: base64</xsl:text>		
		<xsl:text>&#xD;&#xA;</xsl:text>			
		<xsl:text>&#xD;&#xA;</xsl:text>	
		<xsl:value-of select="/eml/body/@value" />
		<xsl:text>&#xD;&#xA;</xsl:text>			
		<xsl:text>&#xD;&#xA;</xsl:text>	
					
		<xsl:for-each select="/eml/attachments/attachment">
			<xsl:text>--</xsl:text><xsl:value-of select="/eml/attachments/@boundary" />
			<xsl:text>&#xD;&#xA;</xsl:text>
			<xsl:text>Content-Type: application/octet-stream;</xsl:text> 
			<xsl:text>&#xD;&#xA;</xsl:text>			
			<xsl:text> name=&quot;</xsl:text><xsl:value-of select="./@rfc" /><xsl:text>&quot;</xsl:text>
			<xsl:text>&#xD;&#xA;</xsl:text>	
			<xsl:text>Content-Disposition: attachment;</xsl:text> 	
			<xsl:text>&#xD;&#xA;</xsl:text>	
			<xsl:text> filename*=&quot;utf-8''</xsl:text><xsl:value-of select="./@uri" /><xsl:text>&quot;;</xsl:text>	
			<xsl:text>&#xD;&#xA;</xsl:text>	
			<xsl:text> size=</xsl:text><xsl:value-of select="./@size" /><xsl:text></xsl:text>
			<xsl:text>&#xD;&#xA;</xsl:text>	
			<xsl:text>Content-Transfer-Encoding: base64</xsl:text>	
			<xsl:text>&#xD;&#xA;</xsl:text>	
			<xsl:text>&#xD;&#xA;</xsl:text>	
			<xsl:value-of select="./@data" />
			<xsl:text>&#xD;&#xA;</xsl:text>	
			<xsl:text>&#xD;&#xA;</xsl:text>																																								
		</xsl:for-each>			

		<xsl:text>--</xsl:text><xsl:value-of select="/eml/attachments/@boundary" /><xsl:text>--</xsl:text>							
	</xsl:when>
	<xsl:otherwise>
		<xsl:value-of select="/eml/body/@value" />
	</xsl:otherwise>
	</xsl:choose>

			
</xsl:template>
									
</xsl:stylesheet>