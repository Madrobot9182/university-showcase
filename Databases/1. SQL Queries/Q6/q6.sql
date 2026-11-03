SELECT k.book_id, title, COUNT(*) as wcount
FROM books k, borrowings b
FULL JOIN waitlists w ON b.book_id = w.book_id
WHERE k.book_id = b.book_id
AND k.pyear <= 2015
GROUP BY k.book_id
HAVING COUNT(k.book_id) >= 1
AND COUNT(k.book_id) >= (COUNT(w.book_id) * 2);
