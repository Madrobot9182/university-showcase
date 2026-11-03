SELECT bid, b.member
FROM borrowings b, waitlists w
WHERE b.book_id = w.book_id
AND julianday(b.end_date) - julianday(b.start_date) > 14
AND w.priority < 5;
