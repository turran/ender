What is it?
===========
Ender is library written in C that describes other libraries. The purpose of Ender is to easily create bindings for any C library without
much struggling.

How?
====
Ender uses an intermediary XML format to describe your library, loads it and registers every definition of your library.
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

Documentation
=============
For API reference check out the automatically generated [doxygen](https://turran.github.io/ender/docs/index.html) documentation

Take a look on the [wiki](https://github.com/turran/ender/wiki) pages for extra information about the project

Available bindings
==================
There are several projects for creating bindings of ender to specific languages. Several are still in deveolping tho

+ [Neko](http://github.com/turran/ender-neko)
+ [JavaScript (SpiderMonkey)](http://github.com/turran/ender-js-sm)
+ [JavaScript (V8)](http://github.com/turran/ender-js-v8)
+ [Lua](http://github.com/turran/ender-lua)
+ [C#](http://github.com/turran/ender-sharp)
+ [Java](http://github.com/turran/ender-java)

FAQ
===
+ Is ender an OO for C? No, it is not. It lets you describe a library that uses any OO system for C (GLib, EO, Enesim Object, your own custom, etc)
