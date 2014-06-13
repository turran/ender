<!-- Extract from a doxygen generated xml doc all the definitions
     needed to create a .ender file
     xsltproc \-\-param lib "'enesim'" \-\-param version 0 \-\-param case "'underscore'" fromdoxygen.xslt doc/xml/index.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="xml" version="1.0" indent="no" standalone="yes" />

  <!-- Our tables to do the lower/upper case -->
  <xsl:variable name="lowercase" select="'abcdefghijklmnopqrstuvwxyz'" />
  <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />

  <!-- function to replace a string http://stackoverflow.com/questions/3067113/xslt-string-replace -->
  <xsl:template name="string-replace-all">
    <xsl:param name="text" />
    <xsl:param name="replace" />
    <xsl:param name="by" />
    <xsl:choose>
      <xsl:when test="contains($text, $replace)">
        <xsl:value-of select="substring-before($text,$replace)" />
        <xsl:value-of select="$by" />
        <xsl:call-template name="string-replace-all">
          <xsl:with-param name="text" select="substring-after($text,$replace)" />
          <xsl:with-param name="replace" select="$replace" />
          <xsl:with-param name="by" select="$by" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$text" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- main entry point -->
  <xsl:template match="/">
    <lib name="{$lib}" version="{$version}" case="{$case}">
      <xsl:apply-templates select="doxygenindex/compound[@kind='struct']"/>
      <xsl:apply-templates select="doxygenindex/compound[@kind='group']/member[@kind='enum']"/>
      <xsl:apply-templates select="doxygenindex/compound[@kind='group']/member[@kind='typedef']"/>
    </lib>
  </xsl:template>

  <!-- convert a type to ender -->
  <xsl:template name="type-to-ender">
    <xsl:param name="text"/>
    <xsl:value-of select="translate(translate($text, '_', '.'), $uppercase, $lowercase)"/>
  </xsl:template>

  <!-- struct type handling -->
  <xsl:template name="field-typification">
    <xsl:param name="text"/>
    <xsl:choose>
      <xsl:when test="$text = 'char *'">
        <xsl:value-of select="'string'"/>
      </xsl:when>
      <!-- int32 variants -->
      <xsl:when test="$text = 'int'">
        <xsl:value-of select="'int32'"/>
      </xsl:when>
      <xsl:when test="$text = 'int32_t'">
        <xsl:value-of select="'int32'"/>
      </xsl:when>
      <!-- int64 variants -->
      <xsl:when test="$text = 'long'">
        <xsl:value-of select="'int64'"/>
      </xsl:when>
      <xsl:when test="$text = 'long int'">
        <xsl:value-of select="'int64'"/>
      </xsl:when>
      <!-- uint32 variants -->
      <xsl:when test="$text = 'unsigned int'">
        <xsl:value-of select="'uint32'"/>
      </xsl:when>
      <xsl:when test="$text = 'uint32_t'">
        <xsl:value-of select="'uint32'"/>
      </xsl:when>
      <!-- uint64 variants -->
      <xsl:when test="$text = 'unsigned long'">
        <xsl:value-of select="'uint64'"/>
      </xsl:when>
      <xsl:when test="$text = 'Eina_Bool'">
        <xsl:value-of select="'bool'"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="translate(translate($text, '_', '.'), $uppercase, $lowercase)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- struct handling -->
  <xsl:template match="compound[@kind='struct']">
    <!-- The name of the struct must be all lower case, replace the '_' by '.', etc -->
    <xsl:variable name="name" select="translate(translate(name/text(), '_', '.'), $uppercase, $lowercase)"/>
    <struct name="{$name}">
      <xsl:apply-templates select="member"/>
    </struct>
  </xsl:template>

  <!-- field handling -->
  <xsl:template match="member[@kind='variable']">
     <xsl:variable name="name" select="name/text()"/>
    <xsl:variable name="id" select="@refid"/>
    <xsl:variable name="fieldtype" select="document(concat(../@refid, '.xml'), ../@refid)//memberdef[@id=$id]/type//text()"/>
    <xsl:variable name="type">
      <xsl:call-template name="field-typification">
        <xsl:with-param name="text" select="$fieldtype"/>
      </xsl:call-template>
    </xsl:variable>
    <field name="{$name}" type="{$type}"/>
  </xsl:template>

  <!-- enums handling -->
  <xsl:template match="member[@kind='enum']">
    <xsl:variable name="name">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="name/text()"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="id" select="@refid"/>
    <enum name="{$name}">
      <!-- pick every enumvalue and pass the parent name to temove it from the generated names -->
      <xsl:apply-templates select="document(concat(../@refid, '.xml'), ../@refid)//memberdef[@id=$id]/enumvalue">
        <xsl:with-param name="pname" select="$name"/>
      </xsl:apply-templates>
    </enum>
  </xsl:template>

  <!-- enum value -->
  <xsl:template match="enumvalue">
    <xsl:param name="pname"/>
    <xsl:variable name="valuename">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="name/text()"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="name">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="$valuename"/>
        <xsl:with-param name="replace" select="concat($pname, '.')"/>
        <xsl:with-param name="by" select="''"/>
      </xsl:call-template>
    </xsl:variable>
    <value name="{$name}"/>
  </xsl:template>

  <!-- params -->
  <xsl:template match="param">
    <xsl:variable name="name" select="declname/text()"/>
    <xsl:variable name="type">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="string(type)"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:if test="not($type = 'void')">
      <arg name="{$name}" type="{$type}"/>
    </xsl:if>
  </xsl:template>

  <!-- method handling -->
  <xsl:template match="member[@kind='function']">
    <xsl:param name="pname"/>
    <xsl:variable name="id" select="@refid"/>
    <xsl:variable name="valuename">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="name/text()"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="name">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="$valuename"/>
        <xsl:with-param name="replace" select="concat($pname, '.')"/>
        <xsl:with-param name="by" select="''"/>
      </xsl:call-template>
    </xsl:variable>
    <method name="{$name}">
      <!-- get the args -->
      <xsl:apply-templates select="document(concat(../@refid, '.xml'), ../@refid)//memberdef[@id=$id]//param"/>
    </method>
  </xsl:template>

  <xsl:template match="member[@kind='typedef']">
    <!-- typedefs can be or either functions, new types or objects. object must be a typedef of an struct -->
    <xsl:variable name="name">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="name/text()"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- doccment() with two arguments: the first the path/node set, the second the node set to take the base uri from -->
    <xsl:variable name="id" select="@refid"/>
    <xsl:variable name="type" select="document(concat(../@refid, '.xml'), ../@refid)//memberdef[@id=$id]/type/text()"/>
    <xsl:choose>
      <xsl:when test="contains($type, 'struct _')">
        <object name="{$name}">
          <!-- get every function that belongs to this group -->
          <xsl:apply-templates select="../descendant::member[@kind='function']">
            <xsl:with-param name="pname" select="$name"/>
          </xsl:apply-templates>
        </object>
      </xsl:when>
      <xsl:otherwise>
        <unknown type="{$type}"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
