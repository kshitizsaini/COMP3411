removedup([],[],_).
removedup([First|Next],List, Num) :-
	List = [First|Next],
	First \= Num.
	
removedup([First|Next],List, Num) :-
	removedup(Next, R, Num),
	List = R,
	First = Num.
