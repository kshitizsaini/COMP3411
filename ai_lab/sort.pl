% base case
insert(Num, [], NewList) :-
	NewList = [Num],
	!.
% recursive case	
insert(Num, List, NewList) :-
	List = [Head|_],
	Num =< Head,
	NewList = [Num|List],
	!.

insert(Num, List, NewList) :-
	List = [Head|Tail],
	Num > Head,
	insert(Num, Tail, R),
	NewList = [Head|R],
	!.

isort([],[]).
isort(List, NewList) :-
	List = [Head|Tail],
	isort(Tail, R),
	insert(Head, R, NewList),!.

split([],[],[]).
split([Num],[Num],[]).
split(BigList, List1, List2) :-
	BigList = [Head|Tail],
	Tail = [Head2|Tail2],
	split(Tail2, List12, List22),
	List1 = [Head|List12],
	List2 = [Head2|List22],
	!.
% base case
merge([],[],[]).
merge(Sort1, [], Sort1):-
	Sort1 \= [],
	!.
merge([],Sort2, Sort2):-
	Sort2 \= [],
	!.
% recursive case
merge(Sort1, Sort2, Sort) :-
	Sort1 = [Head1|Tail1],
	Sort2 = [Head2|_],
	Head1 =< Head2,
	merge(Tail1, Sort2, R),
	Sort = [Head1|R],
	!.

merge(Sort1, Sort2, Sort) :-
	Sort1 = [Head1|_],
	Sort2 = [Head2|Tail2],
	Head1 > Head2,
	merge(Sort1, Tail2, R),
	Sort = [Head2|R],
	!.

mergesort([], []) :-
	!.
mergesort([Num], [Num]):-
	!.
mergesort(List, NewList):-
	split(List, List1, List2),
	mergesort(List1, R1),
	mergesort(List2, R2),	
	merge(R1, R2, NewList),
	!.
	
