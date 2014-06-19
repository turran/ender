What is it
==========
Ender is library written in C that describes other libraries. The purpose of Ender is to easily create bindings for any C library without
much struggling.

How?
====
Ender uses an intermediary XML format to describe your library, lodas it and registers every definition of your library.
For example, for a C library defined like this:

```c
/* contents of foo.h */
typedef struct _Foo_S1 {
  int i;
  char *s;
} Foo_S1;

int foo_s1_has_i(Foo_S1 *thiz);
```

And the corresponding XML's Ender file foo.ender:

```xml
<?xml version="1.0" standalone="yes"?>
<lib name="foo" version="0" case="underscore">
  <!-- Define the struct -->
  <struct name="foo.s1">
    <!-- Now the fields -->
    <field name="i" type="i32"/>
    <field name="s" type="string"/>
    <!-- A method always receives the struct/object as its first argument -->
    <method name="has_i">
      <return type="i32" transfer="full"/>
    </mthod>
  </struct>
</lib>
```

For some big libraries, it might be tedious to do the XML file manually, for that, there is an automatic generator from doxygen
documentation.

Available bindings
==================
There are several projects for creating bindings of ender to specific languages. Several are still in deveolping tho

FAQ
===
+ Is ender an OO for C? No, it is not. It lets you describe a library that uses any OO system for C (GLib, EO, Enesim Object, your own custom, etc)
