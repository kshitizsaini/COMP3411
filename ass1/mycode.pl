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

% parent(a, c).
% parent(b, c).
% parent(c, e).
% parent(d, e).
% parent(c, f).
% parent(d, f).
% parent(i, j).
% parent(e, j).
% parent(f, h).
% parent(g, h).

% male(a).
% male(c).
% male(e).
% male(g).
% male(h).

% female(b).
% female(d).
% female(f).
% female(i).
% female(j).

% oldest is the function that stores the oldest male
% ancestor of a person
% case 1: the male has no parent at all then himself it the oldest along
% this family branch
hasfather(Person) :-
	parent(P, Person),
	male(P).
oldest(Person, Result) :-
	male(Person),
	not(parent(_, Person)),
	Result = Person.
% case 2: the male has no male parent.
oldest(Person, Result) :-
	male(Person),
	not(hasfather(Person)),
	Result = Person.

% otherwise the oldest ancestor of his parent would be the oldest ancestor
oldest(Person, Result) :-
	parent(Person1, Person),
	male(Person1),
	oldest(Person1, R),
	Result = R.

% same_name if a person and himself
same_name(Person, Person).
% same name if it is a father child relationship
same_name(Person1, Person2) :-
	parent(Person1, Person2),
	male(Person1).
same_name(Person1, Person2) :-
	parent(Person2, Person1),
	male(Person2).
% same name if the oldest male ancestor of both are the same
same_name(Person1, Person2) :-
	oldest(Person1, G1),
	oldest(Person2, G2),
	G1 = G2.
	
% helper functions
uniquesign(V1, V2):-
	V1 >= 0,
	V2 >= 0.

uniquesign(V1, V2):-
	V1 < 0,
	V2 < 0.
% helper function to see whether a list is a list with unique sign
uniquelist([],[]).
% only one value yes
uniquelist([Head],[Head]).
% general case
uniquelist(List, Result) :-
	List = [Head | Tail],
	Tail = [Value | _],
	uniquelist(Tail, _),
	uniquesign(Head, Value),
	Result = List.

% the most important helper function, that get the first sign turning point
firstspot(List, Part1, Part2) :-
	List = [Head | Tail],
	Tail = [Value | _],
	not(uniquesign(Head, Value)),
	Part1 = [Head],
	Part2 = Tail.

firstspot(List, Part1, Part2) :-
	List = [Head | Tail],
	Tail = [Value | _],
	uniquesign(Head, Value),
	firstspot(Tail, P11, P2),
	Part1 = [Head | P11],
	Part2 = P2.
% base case for empty list
sign_runs([], []).
% case for a unique list
sign_runs(List, Result) :-
	List \= [],
	uniquelist(List, R),
	Result = [R].

% case for a none unique list find the first invalid spot and make a split
sign_runs(List, Result) :-
	List \= [],
	not(uniquelist(List, _)),
	firstspot(List, R, New),
	sign_runs(New, Return),
	Result = [R | Return].

