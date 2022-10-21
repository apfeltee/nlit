
#**no current plan on continuing**

There is currently a bug that causes code only to run when variables are emitted as SET_GLOBAL, as opposed to SET_LOCAL.  
Needless to say, this declares *everything* as global, in *every scope*. Which is bad. And renders most scripts fairly useless.

Until there is a way to fix that, this repo will be on ice.

----

features:
- full garbage collector (mark & sweep)
- good prototyping system for functions
- syntactically fairly compatible with javascript (i.e., implicit semicolons, et al)
- use of std::(basic_)string internally makes adapting to, and adopting from libraries straight-forward
- good performance
- probably lots more!

Look at the various `*.lit` strewn about to get a handle at what it looks like.