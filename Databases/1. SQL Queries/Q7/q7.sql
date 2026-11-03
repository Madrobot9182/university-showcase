WITH book_cte (book_id, title, pyear, b_rank) AS (
	SELECT b.book_id, title, pyear, RANK() OVER (ORDER BY COUNT(b.book_id) DESC) as b_rank
	FROM books k, borrowings b
	WHERE k.book_id = b.book_id
	GROUP BY b.book_id
)
SELECT book_id, title, pyear from book_cte where b_rank <= 3;
