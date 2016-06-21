# space-colonization-cinder

A C++ implementation of the Space Colonization Algorithm ([paper](http://www.algorithmicbotany.org/papers/colonization.egwnp2007.html)),
for use with [Cinder](https://libcinder.org/).

A port of [this](https://github.com/mikedotalmond/space-colonization), 
which is a port of [this javascript library](https://github.com/nicknikolov/pex-space-colonization) 
by [Nick Nikolov](https://twitter.com/nicknikolov).

Note: Currently does a lot of alloc/delete of the Bud and Hormone structs while iterating - runs very slowly with debug builds.
Feel free to improve it.

