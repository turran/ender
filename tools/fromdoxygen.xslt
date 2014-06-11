<!-- Extract from a doxygen generated xml doc all the definitions
     needed to create a .ender file
     xsltproc fromdoxygen.xslt index.xml > all.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="xml" version="1.0" indent="no" standalone="yes" />
  <!-- Our tables to do the lower/upper case -->
  <xsl:variable name="lowercase" select="'abcdefghijklmnopqrstuvwxyz'" />
  <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />

  <xsl:template match="/">
    <lib name="foo" version="fooversion" case="fooscore">
      <xsl:apply-templates select="doxygenindex/compound[@kind='struct']"/>
      <xsl:apply-templates select="doxygenindex/compound/member[@kind='enum']"/>
    </lib>
  </xsl:template>

  <xsl:template match="compound[@kind='struct']">
    <!-- The name of the struct must be all lower case, replace the '_' by '.', etc -->
    <xsl:variable name="name" select="translate(translate(name/text(), '_', '.'), $uppercase, $lowercase)"/>
    <struct name="{$name}">
    </struct>
  </xsl:template>

  <xsl:template match="compound/member[@kind='enum']">
    <xsl:variable name="name" select="translate(translate(name/text(), '_', '.'), $uppercase, $lowercase)"/>
    <enum name="{$name}">
    </enum>
  </xsl:template>
</xsl:stylesheet>
