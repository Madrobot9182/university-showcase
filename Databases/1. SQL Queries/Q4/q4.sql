SELECT k.book_id, k.title, k.author, COUNT(b.book_id) as bcount, b.start_date as rdate
FROM books k
FULL JOIN borrowings b ON b.book_id = k.book_id
WHERE k.pyear > 2001
GROUP BY k.book_id;
