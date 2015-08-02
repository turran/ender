<!-- Extract from a doxygen generated xml doc all the definitions
     needed to create a .ender file
     xsltproc \-\-param lib "'enesim'" \-\-param version 0 \-\-param case "'underscore'" fromdoxygen.xslt doc/xml/index.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:dyn="http://exslt.org/dynamic" extension-element-prefxes="dyn">
  <xsl:output method="xml" version="1.0" indent="no" standalone="yes" />

  <!-- Our tables to do the lower/upper case -->
  <xsl:variable name="lowercase" select="'abcdefghijklmnopqrstuvwxyz'" />
  <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />
  <!-- Our property key for Muenchian grouping -->
  <xsl:key name="prop" match="memberdef[@kind='function'][//ender-prop]" use=".//ender-prop/@name"/>

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

  <!-- substring before last occurence http://www.heber.it/?p=1278 -->
  <xsl:template name="substring-before-last">
    <!--passed template parameter -->
    <xsl:param name="list"/>
    <xsl:param name="delimiter"/>
    <xsl:choose>
      <xsl:when test="contains($list, $delimiter)">
        <!-- get everything in front of the first delimiter -->
        <xsl:value-of select="substring-before($list,$delimiter)"/>
        <xsl:choose>
          <xsl:when test="contains(substring-after($list,$delimiter),$delimiter)">
            <xsl:value-of select="$delimiter"/>
          </xsl:when>
        </xsl:choose>
        <xsl:call-template name="substring-before-last">
          <!-- store anything left in another variable -->
          <xsl:with-param name="list" select="substring-after($list,$delimiter)"/>
          <xsl:with-param name="delimiter" select="$delimiter"/>
        </xsl:call-template>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <!-- remove the const thing -->
  <xsl:template name="remove-const">
    <xsl:param name="type"/>
    <!-- remove any const thing -->
    <xsl:variable name="no-const-type">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="$type"/>
        <xsl:with-param name="replace" select="'const'"/>
        <xsl:with-param name="by" select="''"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="$no-const-type"/>
  </xsl:template>

  <!-- remove the (* for function pointers -->
  <xsl:template name="remove-fpointer">
    <xsl:param name="type"/>
    <xsl:variable name="no-fpointer-type">
      <xsl:choose>
        <xsl:when test="contains($type, '(*')">
          <xsl:call-template name="string-replace-all">
            <xsl:with-param name="text" select="$type"/>
            <xsl:with-param name="replace" select="'(*'"/>
            <xsl:with-param name="by" select="''"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$type"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="$no-fpointer-type"/>
  </xsl:template>

  <!-- remove the direction pointer -->
  <xsl:template name="remove-direction">
    <xsl:param name="direction"/>
    <xsl:param name="text"/>
    <xsl:variable name="ret">
      <xsl:choose>
        <xsl:when test="$direction = 'out'">
          <xsl:value-of select="normalize-space(substring($text, 1, string-length($text) - 1))"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="normalize-space($text)"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="$ret"/>
  </xsl:template>

  <!-- remove the exported prefix -->
  <xsl:template name="remove-exported">
    <xsl:param name="type"/>
    <!-- remove any const thing -->
    <xsl:variable name="no-exported-type">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="$type"/>
        <xsl:with-param name="replace" select="'EAPI'"/>
        <xsl:with-param name="by" select="''"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="$no-exported-type"/>
  </xsl:template>

  <!-- remove the exported prefix and the const -->
  <xsl:template name="sanitize-prefix">
    <xsl:param name="type"/>
    <!-- remove any const thing -->
    <xsl:variable name="no-const-type">
      <xsl:call-template name="remove-const">
        <xsl:with-param name="type" select="$type"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- remove any const thing -->
    <xsl:variable name="no-exported-type">
      <xsl:call-template name="remove-exported">
        <xsl:with-param name="type" select="$no-const-type"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- sanitize it -->
    <xsl:variable name="sanitized-prefix">
      <xsl:value-of select="$no-exported-type"/>
    </xsl:variable>
    <xsl:value-of select="$sanitized-prefix"/>
  </xsl:template>

  <!-- get the return type of a function that is a ctor, usefull to know if a
       function is a method or not
  -->
  <xsl:template name="ctor-return-type">
    <xsl:param name="ctor"/>
    <xsl:variable name="rtype">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select="string($ctor/type)"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="$rtype"/>
  </xsl:template>

  <!-- given a type name, transform it to ender -->
  <xsl:template name="name-to-ender">
    <xsl:param name="name"/>
    <xsl:choose>
      <xsl:when test="$name = 'char *'">
        <xsl:value-of select="'string'"/>
      </xsl:when>
      <!-- size variants -->
      <xsl:when test="$name = 'size_t'">
        <xsl:value-of select="'size'"/>
      </xsl:when>
      <!-- ssize variants -->
      <xsl:when test="$name = 'ssize_t'">
        <xsl:value-of select="'size'"/>
      </xsl:when>
      <!-- int8 variants -->
      <xsl:when test="$name = 'int8_t'">
        <xsl:value-of select="'int8'"/>
      </xsl:when>
      <!-- int32 variants -->
      <xsl:when test="$name = 'int'">
        <xsl:value-of select="'int32'"/>
      </xsl:when>
      <xsl:when test="$name = 'int32_t'">
        <xsl:value-of select="'int32'"/>
      </xsl:when>
      <!-- int64 variants -->
      <xsl:when test="$name = 'long'">
        <xsl:value-of select="'int64'"/>
      </xsl:when>
      <xsl:when test="$name = 'long int'">
        <xsl:value-of select="'int64'"/>
      </xsl:when>
      <xsl:when test="$name = 'unsigned char'">
        <xsl:value-of select="'uint8'"/>
      </xsl:when>
      <!-- uint8 variants -->
      <xsl:when test="$name = 'uint8_t'">
        <xsl:value-of select="'uint8'"/>
      </xsl:when>
      <!-- uint16 variants -->
      <xsl:when test="$name = 'uint16_t'">
        <xsl:value-of select="'uint16'"/>
      </xsl:when>
      <!-- uint32 variants -->
      <xsl:when test="$name = 'unsigned int'">
        <xsl:value-of select="'uint32'"/>
      </xsl:when>
      <xsl:when test="$name = 'uint32_t'">
        <xsl:value-of select="'uint32'"/>
      </xsl:when>
      <!-- uint64 variants -->
      <xsl:when test="$name = 'unsigned long'">
        <xsl:value-of select="'uint64'"/>
      </xsl:when>
      <xsl:when test="$name = 'Eina_Bool'">
        <xsl:value-of select="'bool'"/>
      </xsl:when>
      <!-- pointer variants -->
      <xsl:when test="$name = 'void *'">
        <xsl:value-of select="'pointer'"/>
      </xsl:when>
      <xsl:when test="$name = 'uint8_t *'">
        <xsl:value-of select="'pointer'"/>
      </xsl:when>
      <xsl:when test="$name = 'uint16_t *'">
        <xsl:value-of select="'pointer'"/>
      </xsl:when>
      <xsl:when test="$name = 'uint32_t *'">
        <xsl:value-of select="'pointer'"/>
      </xsl:when>
      <xsl:otherwise>
        <!-- make it lowercase, change the '_' to '.' and remove any * indirection -->
        <xsl:value-of select="normalize-space(translate(translate(translate($name, '_', '.'), $uppercase, $lowercase), '*', ''))"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- convert a type to ender
       TODO this should go away and use only name-to-ender and match type,
       later, the above function will be renamed to type-to-ender
   -->
  <xsl:template name="type-to-ender">
    <xsl:param name="text"/>
    <!-- remove the EAPI thing -->
    <xsl:variable name="replaced">
      <xsl:call-template name="remove-exported">
        <xsl:with-param name="type" select="$text"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="name" select="normalize-space($replaced)"/>
    <xsl:variable name="type">
      <xsl:call-template name="name-to-ender">
        <xsl:with-param name="name" select="$name"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="$type"/>
  </xsl:template>

  <!-- ender-group ref -->
  <xsl:template match="ender-group/ref">
    <xsl:apply-templates select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef">
      <xsl:with-param name="only-proto" select="false()"/>
    </xsl:apply-templates>
  </xsl:template>

  <!-- for protos -->
  <xsl:template match="ender-group-proto/ref">
    <xsl:apply-templates select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef">
      <xsl:with-param name="only-proto" select="true()"/>
    </xsl:apply-templates>
  </xsl:template>

  <!-- header file -->
  <xsl:template match="compounddef[@kind='file']">
    <!-- first get all the proto -->
    <xsl:apply-templates select=".//ender-group-proto/ref"/>
    <!-- get all the groups with the <ender-group> tag -->
    <xsl:apply-templates select=".//ender-group/ref"/>
  </xsl:template>

  <!-- header ref -->
  <xsl:template match="includes[@local='yes']">
    <!-- get all the header files -->
    <xsl:apply-templates select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef"/>
  </xsl:template>

  <xsl:template match="ender-depends">
    <xsl:variable name="name" select="@from"/>
    <includes name="{$name}"/>
  </xsl:template>

  <!-- main entry point -->
  <xsl:template match="/">
    <lib name="{$lib}" version="{$version}" case="{$case}">
      <!-- first get the dependencies -->
      <xsl:apply-templates select=".//ender-depends"/>
      <!-- We need to get every header, get it and then get every group from such header -->
      <xsl:apply-templates select="doxygen/compounddef/includes[@local='yes']"/>
    </lib>
  </xsl:template>

  <!-- struct handling -->
  <xsl:template match="compounddef[@kind='struct']">
    <xsl:apply-templates select=".//memberdef[@kind='variable']"/>
  </xsl:template>

  <!-- field handling -->
  <xsl:template match="sectiondef[@kind='public-attrib']/memberdef[@kind='variable']">
    <xsl:variable name="name" select="name/text()"/>
    <xsl:variable name="fieldtype" select="type//text()"/>
    <xsl:variable name="type">
      <xsl:apply-templates select="type">
        <xsl:with-param name="direction" select="in"/>
      </xsl:apply-templates>
    </xsl:variable>
    <field name="{$name}" type="{$type}"/>
  </xsl:template>

  <!-- constant handling -->
  <xsl:template match="sectiondef[@kind='var']/memberdef[@kind='variable']">
    <xsl:variable name="type">
      <xsl:apply-templates select="type">
        <xsl:with-param name="direction" select="in"/>
      </xsl:apply-templates>
    </xsl:variable>
    <xsl:variable name="name">
      <xsl:call-template name="member-name">
        <xsl:with-param name="node" select="."/>
      </xsl:call-template>
    </xsl:variable>
    <constant name="{$name}" type="{$type}"/>
  </xsl:template>

  <!-- enums handling -->
  <xsl:template match="memberdef[@kind='enum']">
    <xsl:variable name="name">
      <xsl:call-template name="member-name">
        <xsl:with-param name="node" select="."/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="lower_name" select="translate(name/text(), $uppercase, $lowercase)"/>
    <enum name="{$name}">
      <!-- pick every enumvalue and pass the parent name to temove it from the generated names -->
      <xsl:apply-templates select="enumvalue">
        <xsl:with-param name="pname" select="$lower_name"/>
      </xsl:apply-templates>
      <!-- get every function that belongs to this group -->
      <xsl:apply-templates select="/descendant::memberdef[@kind='function'][not(.//ender-prop)]">
        <xsl:with-param name="pname" select="$lower_name"/>
        <xsl:with-param name="ptype" select="$name"/>
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
    <xsl:element name="value">
      <xsl:attribute name="name">
        <xsl:value-of select="$name"/>
      </xsl:attribute>
      <xsl:if test="initializer">
        <xsl:attribute name="initializer" value="initializer/text()">
          <xsl:variable name="expr">
            <xsl:call-template name="string-replace-all">
              <xsl:with-param name="text" select="initializer/text()"/>
              <xsl:with-param name="replace" select="'='"/>
              <xsl:with-param name="by" select="''"/>
            </xsl:call-template>
          </xsl:variable>
          <xsl:value-of select="dyn:evaluate($expr)"/>
      </xsl:attribute>
      </xsl:if>
    </xsl:element>
  </xsl:template>

  <!-- get the transfer type based on the const attribute -->
  <xsl:template name="param-transfer-type">
    <xsl:param name="type"/>
    <xsl:variable name="no-const-type">
      <xsl:call-template name="remove-const">
        <xsl:with-param name="type" select="$type"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:choose>
      <!-- if they are equal means that no const has been removed and thus a full transfer is done -->
      <xsl:when test="$no-const-type = $type">
        <xsl:value-of select="'full'"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="'none'"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- return value -->
  <xsl:template name="return-item">
    <xsl:param name="node"/>
    <!-- remove the (* for function pointers -->
    <xsl:variable name="name">
      <xsl:call-template name="remove-fpointer">
        <xsl:with-param name="type" select="string($node)"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="direction" select="'in'"/>
    <!-- handle the transfer -->
    <xsl:variable name="transfer-attr" select="$node/../detaileddescription/para/simplesect[@kind='return']//ender-transfer/@type"/>
    <xsl:variable name="transfer">
      <xsl:choose>
        <xsl:when test="string($transfer-attr)">
          <xsl:value-of select="$transfer-attr"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="param-transfer-type">
            <xsl:with-param name="type" select="$name"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <!-- handle the nullable -->
    <xsl:variable name="nullable">
      <xsl:choose>
        <xsl:when test="$node/../detaileddescription/para/simplesect[@kind='return']//ender-nullable">
          <xsl:value-of select="true()"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="false()"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <!-- keep a no const -->
    <xsl:variable name="no-const-type">
      <xsl:call-template name="remove-const">
        <xsl:with-param name="type" select="$name"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="no-space-type" select="normalize-space($no-const-type)"/>
    <!-- get the type -->
    <xsl:variable name="type">
      <xsl:choose>
        <xsl:when test="$node/../detaileddescription//simplesect[@kind='return']//ender-type">
          <xsl:value-of select="$node/../detaileddescription//simplesect[@kind='return']//ender-type/@type"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates select="type">
            <xsl:with-param name="direction" select="$direction"/>
          </xsl:apply-templates>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="not($type = 'void')">
      <return type="{$type}" transfer="{$transfer}" nullable="{$nullable}"/>
    </xsl:if>
  </xsl:template>

  <!-- params -->
  <xsl:template match="param">
    <xsl:param name="skip-first"/>
    <xsl:variable name="name" select="declname/text()"/>
    <!-- the position is relative to the node set, not the real document -->
    <xsl:variable name="pos">
      <xsl:choose>
        <xsl:when test="$skip-first">
          <xsl:value-of select="position() + 1"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="position()"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <!-- handle the direction -->
    <xsl:variable name="direction-attr" select="../detaileddescription/para/parameterlist[@kind='param']/parameteritem[position() = $pos]//parametername/@direction" />
    <xsl:variable name="direction">
      <xsl:choose>
        <xsl:when test="not(string($direction-attr))">in</xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$direction-attr"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <!-- handle the transfer -->
    <xsl:variable name="transfer-attr" select="../detaileddescription/para/parameterlist[@kind='param']/parameteritem[position() = $pos]//ender-transfer/@type"/>
    <xsl:variable name="transfer">
      <xsl:choose>
        <xsl:when test="string($transfer-attr)">
          <xsl:value-of select="$transfer-attr"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="param-transfer-type">
            <xsl:with-param name="type" select="string(type)"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <!-- keep a no const -->
    <xsl:variable name="no-const-type">
      <xsl:call-template name="remove-const">
        <xsl:with-param name="type" select="string(type)"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="no-space-type" select="normalize-space($no-const-type)"/>
    <!-- get the type -->
    <xsl:variable name="type">
      <xsl:choose>
        <xsl:when test="../detaileddescription/para/parameterlist[@kind='param']/parameteritem[position() = $pos]//ender-type">
          <xsl:value-of select="../detaileddescription/para/parameterlist[@kind='param']/parameteritem[position() = $pos]//ender-type/@type"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates select="type">
            <xsl:with-param name="direction" select="$direction"/>
          </xsl:apply-templates>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="not($type = 'void')">
      <xsl:element name="arg">
        <xsl:attribute name="name">
          <xsl:value-of select="$name"/>
        </xsl:attribute>
        <xsl:attribute name="type">
          <xsl:value-of select="$type"/>
        </xsl:attribute>
        <xsl:attribute name="direction">
          <xsl:value-of select="$direction"/>
        </xsl:attribute>
        <xsl:attribute name="transfer">
          <xsl:value-of select="$transfer"/>
        </xsl:attribute>
        <!-- handle the nullable -->
        <xsl:if test="../detaileddescription/para/parameterlist[@kind='param']/parameteritem[position() = $pos]//ender-nullable">
          <xsl:attribute name="nullable">
            <xsl:value-of select="true()"/>
          </xsl:attribute>
        </xsl:if>
        <!-- handle the delayed callback -->
        <xsl:if test="../detaileddescription/para/parameterlist[@kind='param']/parameteritem[position() = $pos]//ender-delayed">
          <xsl:attribute name="delayed-callback">
            <xsl:value-of select="true()"/>
          </xsl:attribute>
        </xsl:if>
      </xsl:element>
    </xsl:if>
  </xsl:template>

  <!-- property handling -->
  <xsl:template match="memberdef[@kind='function'][.//ender-prop]">
    <xsl:param name="oname"/>
    <xsl:param name="otype"/>
    <xsl:variable name="pname" select=".//ender-prop/@name"/>
    <!-- get the second arg of the setter, or the second arg/return value of the getter -->
    <xsl:element name="prop">
      <xsl:attribute name="name">
        <xsl:value-of select="$pname"/>
      </xsl:attribute>
      <!-- check if it is a valueof -->
      <xsl:if test=".//ender-valueof">
        <xsl:attribute name="value-of">
          <xsl:value-of select="'true'"/>
        </xsl:attribute>
      </xsl:if>
      <!-- check if it is a downcast -->
      <xsl:if test=".//ender-downcast">
        <xsl:attribute name="downcast">
          <xsl:value-of select="'true'"/>
        </xsl:attribute>
      </xsl:if>
      <!-- look for setters/getters -->
      <xsl:for-each select="key('prop',.//ender-prop/@name)">
        <!-- get the function name without the property name -->
        <xsl:variable name="lower_name" select="translate(name/text(), $uppercase, $lowercase)"/>
        <xsl:variable name="name">
          <xsl:call-template name="string-replace-all">
            <xsl:with-param name="text" select="$lower_name"/>
            <xsl:with-param name="replace" select="concat($oname, concat(concat('_', $pname), '_'))"/>
            <xsl:with-param name="by" select="''"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:choose>
          <xsl:when test="$name = 'get'">
            <getter>
              <!-- get the return value -->
              <xsl:call-template name="return-item">
                <xsl:with-param name="node" select="./type"/>
              </xsl:call-template>
              <!-- get the args -->
              <xsl:apply-templates select=".//param[position() > 1]">
                <xsl:with-param name="skip-first" select="true()"/>
              </xsl:apply-templates>
            </getter>
          </xsl:when>
          <xsl:when test="$name = 'set'">
            <setter>
              <!-- get the return value -->
              <xsl:call-template name="return-item">
                <xsl:with-param name="node" select="./type"/>
              </xsl:call-template>
              <!-- get the args -->
              <xsl:apply-templates select=".//param[position() > 1]">
                <xsl:with-param name="skip-first" select="true()"/>
              </xsl:apply-templates>
            </setter>
          </xsl:when>
          <xsl:otherwise>
            <accessor name="{$name}" oname="{$oname}" otype="{$otype}"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
    </xsl:element>
  </xsl:template>

  <!-- A function that returns the compound (struct, etc) ender name. If the node has
       an ender-name, that one is used, otherwise, we use the compounddef's text
       node and enderize it
       Param in: the compounddef node of a struct
       Value out: the name
  !-->
  <xsl:template name="compound-name">
    <xsl:param name="node"/>
    <xsl:variable name="name">
      <xsl:choose>
        <xsl:when test="$node/detaileddescription//ender-name">
          <xsl:value-of select="$node/detaileddescription//ender-name/@name"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="name-to-ender">
            <xsl:with-param name="name" select="$node/compoundname/text()"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="$name"/>
  </xsl:template>

  <!-- A function that returns the member (typedef, etc) ender name. If the node has
       an ender-name, that one is used, otherwise, we use the memberdef's name
       node and enderize it
       Param in: the memberdef node
       Value out: the name
  !-->
  <xsl:template name="member-name">
    <xsl:param name="node"/>
    <xsl:variable name="name">
      <xsl:choose>
        <xsl:when test="$node/detaileddescription//ender-name">
          <xsl:value-of select="$node/detaileddescription//ender-name/@name"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="name-to-ender">
            <xsl:with-param name="name" select="$node/name/text()"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="$name"/>
  </xsl:template>

  <!-- Get the name of a type without member reference or compound reference -->
  <xsl:template match="type">
    <xsl:param name="direction"/>
    <!-- remove any function pointer * -->
    <xsl:variable name="fname">
      <xsl:call-template name="remove-fpointer">
        <xsl:with-param name="type" select="string(.)"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- first sanitize -->
    <xsl:variable name="sname">
      <xsl:call-template name="sanitize-prefix">
        <xsl:with-param name="type" select="$fname"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- remove trailing * if is an out type -->
    <xsl:variable name="dname">
      <xsl:call-template name="remove-direction">
        <xsl:with-param name="text" select="$sname"/>
        <xsl:with-param name="direction" select="$direction"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- convert to ender -->
    <xsl:variable name="rname">
      <xsl:call-template name="name-to-ender">
        <xsl:with-param name="name" select="$dname"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="$rname"/>
  </xsl:template>

  <!-- Get the name of a type that has a member reference -->
  <xsl:template match="type[./ref[@kindref='member']]">
    <xsl:param name="direction"/>
    <xsl:variable name="rname">
      <xsl:for-each select="./node()">
        <xsl:choose>
          <xsl:when test="self::text()">
            <xsl:variable name="text">
              <xsl:call-template name="sanitize-prefix">
                <xsl:with-param name="type" select="."/>
              </xsl:call-template>
            </xsl:variable>
            <xsl:value-of select="$text"/>
          </xsl:when>
          <xsl:when test="name() = 'ref'">
            <!-- For a member reference we need to take out the string after the last _ -->
            <xsl:variable name="refid" select="@refid"/>
            <xsl:variable name="ssname">
              <xsl:call-template name="substring-before-last">
                <xsl:with-param name="list" select="@refid"/>
                <xsl:with-param name="delimiter" select="'_'"/>
              </xsl:call-template>
            </xsl:variable>
            <xsl:call-template name="member-name">
              <xsl:with-param name="node" select="document(concat($ssname, '.xml'), @refid)/doxygen/compounddef//memberdef[@id=$refid]"/>
            </xsl:call-template>
          </xsl:when>
        </xsl:choose>
      </xsl:for-each>
    </xsl:variable>
    <!-- remove trailing * -->
    <xsl:variable name="dname">
      <xsl:value-of select="normalize-space(translate($rname, '*', ''))"/>
    </xsl:variable>
    <xsl:value-of select="$dname"/>
  </xsl:template>

  <!-- Get the name of a type that has a compound reference -->
  <xsl:template match="type[./ref[@kindref='compound']]">
    <xsl:param name="direction"/>
    <xsl:variable name="rname">
      <xsl:for-each select="./node()">
        <xsl:choose>
          <xsl:when test="self::text()">
            <xsl:variable name="text">
              <xsl:call-template name="sanitize-prefix">
                <xsl:with-param name="type" select="."/>
              </xsl:call-template>
            </xsl:variable>
            <xsl:value-of select="$text"/>
          </xsl:when>
          <xsl:when test="name() = 'ref'">
            <xsl:call-template name="compound-name">
              <xsl:with-param name="node" select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef"/>
            </xsl:call-template>
          </xsl:when>
        </xsl:choose>
      </xsl:for-each>
    </xsl:variable>
    <!-- remove trailing * if is an out type -->
    <xsl:variable name="dname">
      <xsl:value-of select="normalize-space(translate($rname, '*', ''))"/>
    </xsl:variable>
    <xsl:value-of select="$dname"/>
  </xsl:template>

  <!-- method handling -->
  <xsl:template match="memberdef[@kind='function'][not(.//ender-prop)]">
    <xsl:param name="pname"/>
    <xsl:param name="ptype"/>
    <xsl:variable name="lower_name" select="translate(name/text(), $uppercase, $lowercase)"/>
    <xsl:variable name="name">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="$lower_name"/>
        <xsl:with-param name="replace" select="concat($pname, '_')"/>
        <xsl:with-param name="by" select="''"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- decide to define a method, ctor, ref, unref -->
    <xsl:variable name="first_param" select=".//param[1]"/>
    <xsl:variable name="first_param_type">
      <xsl:apply-templates select="$first_param/type"/>
    </xsl:variable>
    <!--<first-param name="{$first_param_type}"/>-->
    <xsl:choose>
      <!-- for top level functions, use the ender name -->
      <xsl:when test="$pname = 'none'">
        <xsl:variable name="rname">
          <xsl:choose>
            <xsl:when test=".//ender-name">
              <xsl:value-of select=".//ender-name/@name"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:call-template name="type-to-ender">
                <xsl:with-param name="text" select="string($name)"/>
              </xsl:call-template>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <function name="{$rname}">
          <!-- get the return value -->
          <xsl:call-template name="return-item">
            <xsl:with-param name="node" select="./type"/>
          </xsl:call-template>
          <!-- get the args -->
          <xsl:apply-templates select=".//param">
            <xsl:with-param name="skip-first" select="false()"/>
          </xsl:apply-templates>
        </function>
      </xsl:when>
      <!-- ctors -->
      <xsl:when test="contains($name, 'new')">
        <ctor name="{$name}">
          <!-- get the args -->
          <xsl:apply-templates select=".//param">
            <xsl:with-param name="skip-first" select="false()"/>
          </xsl:apply-templates>
        </ctor>
      </xsl:when>
      <!-- unref -->
      <xsl:when test="$name = 'unref'">
        <unref name="unref">
          <!-- get the return value -->
          <xsl:call-template name="return-item">
            <xsl:with-param name="node" select="./type"/>
          </xsl:call-template>
          <!-- get the args -->
          <xsl:apply-templates select=".//param[position() > 1]">
            <xsl:with-param name="skip-first" select="true()"/>
          </xsl:apply-templates>
        </unref>
      </xsl:when>
      <!-- ref -->
      <xsl:when test="$name = 'ref'">
        <ref name="ref">
          <!-- get the return value -->
          <xsl:call-template name="return-item">
            <xsl:with-param name="node" select="./type"/>
          </xsl:call-template>
          <!-- get the args -->
          <xsl:apply-templates select=".//param[position() > 1]">
            <xsl:with-param name="skip-first" select="true()"/>
          </xsl:apply-templates>
        </ref>
      </xsl:when>
      <xsl:when test="$first_param_type = $ptype">
        <xsl:element name="method">
          <xsl:attribute name="name">
            <xsl:value-of select="$name"/>
          </xsl:attribute>
          <!-- the flags -->
          <xsl:if test=".//ender-downcast">
            <xsl:attribute name="downcast">
              <xsl:value-of select="'true'"/>
            </xsl:attribute>
          </xsl:if>
          <!-- get the return value -->
          <xsl:call-template name="return-item">
            <xsl:with-param name="node" select="./type"/>
          </xsl:call-template>
          <!-- get the args -->
          <xsl:apply-templates select=".//param[position() > 1]">
            <xsl:with-param name="skip-first" select="true()"/>
          </xsl:apply-templates>
        </xsl:element>
      </xsl:when>
      <xsl:otherwise>
        <function name="{$name}">
          <!-- get the return value -->
          <xsl:call-template name="return-item">
            <xsl:with-param name="node" select="./type"/>
          </xsl:call-template>
          <!-- get the args -->
          <xsl:apply-templates select=".//param">
            <xsl:with-param name="skip-first" select="false()"/>
          </xsl:apply-templates>
        </function>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="memberdef[@kind='typedef']">
    <xsl:param name="only-proto"/>
    <!-- check if the definition has an inherits tag -->
    <xsl:variable name="itype">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select=".//ender-inherits/@from"/>
      </xsl:call-template>
    </xsl:variable>
    <!-- typedefs can be or either functions, new types or objects. object must be a typedef of an struct -->
    <xsl:variable name="lower_name" select="translate(name/text(), $uppercase, $lowercase)"/>
    <xsl:variable name="name">
      <xsl:call-template name="member-name">
        <xsl:with-param name="node" select="."/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="type" select="type/text()"/>
    <xsl:choose>
      <!-- object -->
      <xsl:when test="contains($type, 'struct _')">
        <!-- decide to use the inherit or not -->
        <xsl:element name="object">
          <xsl:attribute name="name">
            <xsl:value-of select="$name"/>
          </xsl:attribute>
          <xsl:if test="string($itype)">
            <xsl:attribute name="inherits">
              <xsl:value-of select="$itype"/>
            </xsl:attribute>
          </xsl:if>
          <xsl:if test="not($only-proto)">
            <!-- get every function that belongs to this group -->
            <xsl:apply-templates select="/descendant::memberdef[@kind='function'][not(.//ender-prop)]">
              <xsl:with-param name="pname" select="$lower_name"/>
              <xsl:with-param name="ptype" select="$name"/>
            </xsl:apply-templates>
            <!-- get every property -->
            <xsl:apply-templates select="/descendant::memberdef[@kind='function'][.//ender-prop][generate-id(.)=generate-id(key('prop',.//ender-prop/@name)[1])]">
              <xsl:with-param name="oname" select="$lower_name"/>
              <xsl:with-param name="otype" select="$name"/>
            </xsl:apply-templates>
          </xsl:if>
        </xsl:element>
      </xsl:when>
      <!-- function pointer -->
      <xsl:when test="contains($type, '(*')">
        <callback name="{$name}">
          <!-- get the return value -->
          <xsl:call-template name="return-item">
            <xsl:with-param name="node" select="./type"/>
          </xsl:call-template>
          <!-- get the args -->
          <xsl:apply-templates select=".//param">
            <xsl:with-param name="skip-first" select="false()"/>
          </xsl:apply-templates>
        </callback>
      </xsl:when>
      <xsl:otherwise>
        <xsl:variable name="dtype">
          <xsl:call-template name="type-to-ender">
            <xsl:with-param name="text" select="$type"/>
          </xsl:call-template>
        </xsl:variable>
        <def name="{$name}" type="{$dtype}">
          <xsl:if test="not($only-proto)">
            <!-- get every function that belongs to this group -->
            <xsl:apply-templates select="/descendant::memberdef[@kind='function']">
              <xsl:with-param name="pname" select="$lower_name"/>
              <xsl:with-param name="ptype" select="'none'"/>
            </xsl:apply-templates>
          </xsl:if>
        </def>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process innerclasses, i.e structs -->
  <xsl:template match="innerclass">
    <!-- The name of the struct must be all lower case, replace the '_' by '.', etc -->
    <xsl:variable name="lower_name" select="translate(text(), $uppercase, $lowercase)"/>
    <xsl:variable name="refid" select="@refid"/>
    <xsl:variable name="name">
      <xsl:call-template name="compound-name">
        <xsl:with-param name="node" select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef[@id=$refid]"/>
      </xsl:call-template>
    </xsl:variable>
    <struct name="{$name}">
      <!-- the fields -->
      <xsl:apply-templates select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef[@kind='struct']"/>
      <!-- the functions -->
      <xsl:apply-templates select="..//memberdef[@kind='function']">
        <xsl:with-param name="pname" select="$lower_name"/>
        <xsl:with-param name="ptype" select="$name"/>
      </xsl:apply-templates>
    </struct>
  </xsl:template>

  <!-- Process a group -->
  <xsl:template match="doxygen/compounddef[@kind='group']">
    <xsl:param name="only-proto"/>
    <!-- check if the definition has an inherits tag -->
    <xsl:variable name="itype">
      <xsl:call-template name="type-to-ender">
        <xsl:with-param name="text" select=".//ender-inherits/@from"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="lower_name" select="translate(compoundname/text(), $uppercase, $lowercase)"/>
    <xsl:variable name="name">
      <xsl:call-template name="compound-name">
        <xsl:with-param name="node" select="."/>
      </xsl:call-template>
    </xsl:variable>
    <!-- check that we have a valid struct/enum/def -->
    <xsl:variable name="is-main" select="contains($lower_name, 'main')"/>
    <xsl:variable name="has-enum" select="count(.//memberdef[@kind='enum']) > 0"/>
    <xsl:variable name="has-struct" select="count(.//innerclass) > 0"/>
    <xsl:variable name="has-def" select="count(.//memberdef[@kind='typedef']) > 0"/>
    <xsl:choose>
      <!-- main library functions -->
      <xsl:when test="$is-main">
        <xsl:apply-templates select=".//memberdef[@kind='function']">
          <xsl:with-param name="pname" select="'none'"/>
          <xsl:with-param name="ptype" select="'none'"/>
        </xsl:apply-templates>
      </xsl:when>
      <!-- we define an object with the name of the group -->
      <xsl:when test="not($has-enum or $has-struct or $has-def)">
        <!-- get the return type of a constructor in case it has it -->
        <xsl:variable name="rtype">
          <xsl:call-template name="ctor-return-type">
            <xsl:with-param name="ctor" select=".//memberdef[@kind='function'][contains(./name/text(), 'new')][1]"/>
          </xsl:call-template>
        </xsl:variable>
        <!-- choose to use the rtype, the itype or the name -->
        <xsl:variable name="ptype">
          <xsl:choose>
            <xsl:when test="string($rtype)">
              <xsl:value-of select="$rtype"/>
            </xsl:when>
            <xsl:when test="string($itype)">
              <xsl:value-of select="$itype"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$name"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <!-- decide to use the inherit or not -->
        <xsl:element name="object">
          <xsl:attribute name="name">
            <xsl:value-of select="$name"/>
          </xsl:attribute>
          <xsl:if test="string($itype)">
            <xsl:attribute name="inherits">
              <xsl:value-of select="$itype"/>
            </xsl:attribute>
          </xsl:if>
          <xsl:if test="not($only-proto)">
            <!-- now the properties -->
            <xsl:apply-templates select=".//memberdef[@kind='function'][.//ender-prop][generate-id(.)=generate-id(key('prop',.//ender-prop/@name)[1])]">
              <xsl:with-param name="oname" select="$lower_name"/>
              <xsl:with-param name="otype" select="$ptype"/>
            </xsl:apply-templates>
            <!-- now the methods that are not properties -->
            <xsl:apply-templates select=".//memberdef[@kind='function'][not(.//ender-prop)]">
              <xsl:with-param name="pname" select="$lower_name"/>
              <xsl:with-param name="ptype" select="$ptype"/>
            </xsl:apply-templates>
          </xsl:if>
        </xsl:element>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="./innerclass"/>
        <xsl:apply-templates select=".//memberdef[@kind='variable']"/>
        <xsl:apply-templates select=".//memberdef[@kind='enum']"/>
        <xsl:apply-templates select=".//memberdef[@kind='typedef']">
          <xsl:with-param name="only-proto" select="$only-proto"/>
        </xsl:apply-templates>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process every group -->
  <xsl:template match="doxygenindex/compound[@kind='group']">
    <!-- document() with two arguments: the first the path/node set, the second the node set to take the base uri from -->
    <xsl:apply-templates select="document(concat(@refid, '.xml'), @refid)/doxygen/compounddef"/>
  </xsl:template>
</xsl:stylesheet>
