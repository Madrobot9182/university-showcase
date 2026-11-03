SELECT k.book_id, title, AVG(rating) as raverage
FROM books k, reviews r
WHERE k.book_id = r.book_id
GROUP BY r.book_id
HAVING COUNT(*) >= 2
LIMIT 3;
