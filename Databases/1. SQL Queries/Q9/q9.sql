CREATE VIEW book_view(book_id, title, revcnt, rating, rating23, reqcnt)
AS SELECT k.book_id, k.title, COUNT(rating), ifnull(AVG(rating), 0),
	(SELECT ifnull(AVG(rating), 0) WHERE r.book_id = k.book_id AND strftime('%Y', rdate) = '2023'),
	(SELECT COUNT(*) FROM borrowings b WHERE k.book_id = b.book_id) + (SELECT COUNT(*) FROM waitlists w WHERE k.book_id = w.book_id)
	FROM books k
	FULL JOIN reviews r ON k.book_id = r.book_id
	FULL JOIN waitlists w ON k.book_id = w.book_id
	GROUP BY k.book_id;

SELECT * FROM book_view;
