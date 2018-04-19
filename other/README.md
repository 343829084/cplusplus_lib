1>mulitiKey map   多个key做为map的关键字,可以用boost::tuple ，要重写boost::unordermap构造函数的生成hash,及比较相等的二个函数,这里用std::map来实现，重写它的构造函数中的比较函数

