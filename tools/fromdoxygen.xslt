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
      <xsl:apply-templates select="doxygenindex/compound[@kind='group']"/>
    </lib>
  </xsl:template>

  <!-- convert a type to ender -->
  <xsl:template name="type-to-ender">
    <xsl:param name="text"/>
    <xsl:choose>
      <xsl:when test="$text = 'char *'">
        <xsl:value-of select="'string'"/>
      </xsl:when>
      <!-- int8 variants -->
      <xsl:when test="$text = 'int8_t'">
        <xsl:value-of select="'int8'"/>
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
      <!-- uint8 variants -->
      <xsl:when test="$text = 'uint8_t'">
        <xsl:value-of select="'uint8'"/>
      </xsl:when>
      <!-- uint8 variants -->
      <xsl:when test="$text = 'uint16_t'">
        <xsl:value-of select="'uint16'"/>
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
      <!-- pointer variants -->
      <xsl:when test="$text = 'void *'">
        <xsl:value-of select="'pointer'"/>
      </xsl:when>
      <xsl:otherwise>
        <!-- make it lowercase, change the '_' to '.' and remove any * indirection -->
        <xsl:value-of select="normalize-space(translate(translate(translate($text, '_', '.'), $uppercase, $lowercase), '*', ''))"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- struct handling -->
  <xsl:template match="compounddef[@kind='struct']">
    <!-- The name of the struct must be all lower case, replace the '_' by '.', etc -->
    <xsl:variable name="name" select="translate(translate(compoundname/text(), '_', '.'), $uppercase, $lowercase)"/>
    <struct name="{$name}">
      <xsl:apply-templates select=".//memberdef[@kind='variable']"/>
    </struct>
  </xsl:template>

  <!-- field handling -->
  <xsl:template match="memberdef[@kind='variable']">
     <xsl:variable name="name" select="name/text()"/>
    <xsl:variable name="id" select="@refid"/>
    <xsl:variable name="fieldtype" select="type//text()"/>
    <xsl:variable name="type">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="$fieldtype"/>
      </xsl:call-template>
    </xsl:variable>
    <field name="{$name}" type="{$type}"/>
  </xsl:template>

  <!-- enums handling -->
  <xsl:template match="memberdef[@kind='enum']">
    <xsl:variable name="name">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="name/text()"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="id" select="@refid"/>
    <enum name="{$name}">
      <!-- pick every enumvalue and pass the parent name to temove it from the generated names -->
      <xsl:apply-templates select="enumvalue">
        <xsl:with-param name="pname" select="translate(name/text(), $uppercase, $lowercase)"/>
      </xsl:apply-templates>
    </enum>
  </xsl:template>

  <!-- enum value -->
  <xsl:template match="enumvalue">
    <!-- the pname must be the typeof the parent enum, lowercased -->
    <xsl:param name="pname"/>
    <xsl:variable name="lower_name" select="translate(name/text(), $uppercase, $lowercase)"/>
    <xsl:variable name="name">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="$lower_name"/>
        <xsl:with-param name="replace" select="concat($pname, '_')"/>
        <xsl:with-param name="by" select="''"/>
      </xsl:call-template>
    </xsl:variable>
    <value name="{$name}"/>
  </xsl:template>

  <!-- params -->
  <xsl:template match="param">
    <xsl:variable name="name" select="declname/text()"/>
    <xsl:variable name="pos" select="position()"/>
    <!-- handle the direction -->
    <xsl:variable name="direction-attr" select="../detaileddescription/para/parameterlist/parameteritem[$pos + 1]//parametername/@direction" />
    <xsl:variable name="direction">
      <xsl:choose>
        <xsl:when test="not(string($direction-attr))">in</xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$direction-attr"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <!-- remove any const thing -->
    <xsl:variable name="no-const-type">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="string(type)"/>
        <xsl:with-param name="replace" select="'const'"/>
        <xsl:with-param name="by" select="''"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- handle the transfer -->
    <xsl:variable name="transfer">
      <xsl:choose>
        <!-- if they are equal means that no const has been removed and thus a full transfer is done -->
        <xsl:when test="$no-const-type = string(type)">
          <xsl:value-of select="'full'"/> 
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'none'"/> 
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="no-space-type" select="normalize-space($no-const-type)"/>
    <!-- in case is an out direction, remove one pointer -->
    <xsl:variable name="type">
      <xsl:choose>
        <xsl:when test="$direction = 'in'">
          <xsl:call-template name="type-to-ender">
            <xsl:with-param name="text" select="$no-space-type"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="type-to-ender">
            <xsl:with-param name="text" select="normalize-space(substring(string($no-space-type), 1,string-length(string($no-space-type)) - 1))"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="not($type = 'void')">
      <arg name="{$name}" type="{$type}" direction="{$direction}" transfer="{$transfer}"/>
    </xsl:if>
  </xsl:template>

  <!-- method handling -->
  <xsl:template match="memberdef[@kind='function']">
    <xsl:param name="pname"/>
    <xsl:param name="ptype"/>
    <xsl:variable name="id" select="@refid"/>
    <xsl:variable name="lower_name" select="translate(name/text(), $uppercase, $lowercase)"/>
    <xsl:variable name="name">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="$lower_name"/>
        <xsl:with-param name="replace" select="concat($pname, '_')"/>
        <xsl:with-param name="by" select="''"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- decide to define a method, ctor or property -->
    <xsl:variable name="first_param" select=".//param[1]"/>
    <xsl:variable name="first_param_type">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="string($first_param/type)"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="contains($first_param_type, $ptype)">
        <method name="{$name}">
          <!-- get the args -->
          <xsl:apply-templates select=".//param[position() > 1]"/>
        </method>
      </xsl:when>
      <xsl:when test="contains($name, 'new')">
        <ctor>
          <!-- get the args -->
          <xsl:apply-templates select=".//param"/>
        </ctor>
      </xsl:when>
      <xsl:otherwise>
        <function name="{$name}">
          <!-- get the args -->
          <xsl:apply-templates select=".//param"/>
        </function>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="memberdef[@kind='typedef']">
    <!-- typedefs can be or either functions, new types or objects. object must be a typedef of an struct -->
    <xsl:variable name="lower_name" select="translate(name/text(), $uppercase, $lowercase)"/>
    <xsl:variable name="name">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="name/text()"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="id" select="@refid"/>
    <xsl:variable name="type" select="type/text()"/>
    <xsl:choose>
      <xsl:when test="contains($type, 'struct _')">
        <object name="{$name}">
          <!-- get every function that belongs to this group -->
          <xsl:apply-templates select="/descendant::memberdef[@kind='function']">
            <xsl:with-param name="pname" select="$lower_name"/>
            <xsl:with-param name="ptype" select="$name"/>
          </xsl:apply-templates>
        </object>
      </xsl:when>
      <xsl:otherwise>
        <unknown type="{$type}"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process innerclasses -->
  <xsl:template match="innerclass">
    <xsl:apply-templates select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef[@kind='struct']"/>
  </xsl:template>

  <!-- Process a group -->
  <xsl:template match="doxygen/compounddef[@kind='group']">
    <xsl:variable name="lower_name" select="translate(compoundname/text(), $uppercase, $lowercase)"/>
    <xsl:variable name="name">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="compoundname/text()"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- check that we have a valid struct/enum -->
    <xsl:variable name="is-enum" select="contains(.//memberdef[@kind='typedef']/type/text(), 'enum')"/>
    <xsl:variable name="is-struct" select="contains(.//memberdef[@kind='typedef']/type/text(), 'struct')"/>
    <xsl:choose>
      <xsl:when test="not($is-enum or $is-struct)">
        <xsl:apply-templates select=".//memberdef[@kind='enum']"/>
        <object name="{$name}">
          <!-- now the methods -->
          <xsl:apply-templates select=".//memberdef[@kind='function']">
            <xsl:with-param name="pname" select="$lower_name"/>
            <xsl:with-param name="ptype" select="$name"/>
          </xsl:apply-templates>
        </object>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="./innerclass"/>
        <xsl:apply-templates select=".//memberdef[@kind='enum']"/>
        <xsl:apply-templates select=".//memberdef[@kind='typedef']"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process every group -->
  <xsl:template match="doxygenindex/compound[@kind='group']">
    <!-- document() with two arguments: the first the path/node set, the second the node set to take the base uri from -->
    <xsl:apply-templates select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef"/>
  </xsl:template>
</xsl:stylesheet>
