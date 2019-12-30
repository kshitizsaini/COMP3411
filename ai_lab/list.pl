is_list([]).
is_list(.(_, Tail)):-
	is_list(Tail).

head_tail(List, Head, Tail):-
	List = [Head|Tail].

is_member(Element, list(Element, _)).
is_member(Element, list(_, Tail)) :-
	is_member(Element, Tail).

% sumsq_even(List, Sum) :- will store the square of all
% the even values in the List to Sum.

% base case:

sumsq_even([], 0).

% recurrsive case:
% we use a little trick here, (Head + 1) mod 2 returns 1 when we meet even
% numbers and returns 0 when meeting odd numbers

sumsq_even(List, Sum):-
	List = [Head | Tail],
	sumsq_even(Tail, Sum1),
	Add is ((Head + 1) mod 2) * Head * Head, 
	Sum is  Sum1 + Add.

% sqrt_list(List, Result) :- if the list is empty
% then we return an empty list
sqrt_list([], []).

% otherwise, we extract the head node then create a node [head, sqrt(head)]
% finally, we merge this node with the recurrsive case list to get Result.
sqrt_list(List, Result) :-
	List = [Head | Tail],
	sqrt_list(Tail, R),
	Ans is sqrt(Head),
	Node = [Head, Ans],
	Result = [Node | R].

% base case 1: the tree is empty it is a heap.
is_heap(empty).

% base case 2 : the tree has no child it is a heap.
is_heap(tree(empty, _, empty)).

% general case 1: the tree has only right child
is_heap(tree(empty, NUM, R)):-
	R = tree(RL, VR, RR),
	VR >= NUM,
	is_heap(tree(RL, VR, RR)).

% general case 2: the tree has only left child
is_heap(tree(L, NUM, empty)):-
	L = tree(LL, VL, LR),
	VL >= NUM,
	is_heap(tree(LL, VL, LR)).

% general case 3 : the tree has both left child and right child
is_heap(tree(L, NUM, R)):-
	L = tree(LL, VL, LR),
	R = tree(RL, VR, RR),
	VL >= NUM,
	VR >= NUM,
	is_heap(tree(LL, VL, LR)),
	is_heap(tree(RL, VR, RR)).
	
	
	
