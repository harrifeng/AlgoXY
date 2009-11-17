type Key = String

data Patricia a = Patricia { value :: Maybe a
                           , children :: [(Key, Patricia a)]}

empty = Patricia Nothing []

leaf :: a -> Patricia a
leaf x = Patricia (Just x) []

--insert
insert :: Patricia a -> Key -> a -> Patricia a
insert t k x = Patricia (value t) (ins (children t) k x) where
    ins []     k x = [(k, Patricia (Just x) [])]
    ins (p:ps) k x 
        | (fst p) == k 
            = (k, Patricia (Just x) (children (snd p))):ps --overwrite
        | match (fst p) k 
            = (branch k x (fst p) (snd p)):ps
        | otherwise 
            = p:(ins ps k x)

match :: Key -> Key -> Bool
match [] _ = False
match _ [] = False
match x y = head x == head y

branch :: Key -> a -> Key -> Patricia a -> (Key, Patricia a)
branch k1 x k2 t2 
    | k1 == k 
        -- ex: insert "an" into "another" 
        = (k, Patricia (Just x) [(k2', t2)])
    | k2 == k 
        -- ex: insert "another" into "an"
        = (k, insert t2 k1' x)
    | otherwise = (k, Patricia Nothing [(k1', leaf x), (k2', t2)]) 
   where
      k = lcp k1 k2
      k1' = drop (length k) k1
      k2' = drop (length k) k2

lcp :: Key -> Key -> Key
lcp [] _ = []
lcp _ [] = []
lcp (x:xs) (y:ys) = if x==y then x:(lcp xs ys) else []

fromList :: [(Key, a)] -> Patricia a
fromList xs = foldl ins empty xs where
    ins t (k, v) = insert t k v

toString :: (Show a)=>Patricia a -> String
toString t = toStr t "" where
    toStr t prefix = "(" ++ prefix ++ showMaybe (value t) ++
                     (concat $ map (\(k, v)->", " ++ toStr v (prefix++k))
                             (children t))
                     ++ ")"
    showMaybe Nothing = ""
    showMaybe (Just x) = ":" ++ show x

testPatricia = "t1=" ++ (toString t1) ++ "\n" ++
               "t2=" ++ (toString t2)
    where
      t1 = fromList [("a", 1), ("an", 2), ("another", 7), ("boy", 3), ("bool", 4), ("zoo", 3)]
      t2 = fromList [("zoo", 3), ("bool", 4), ("boy", 3), ("another", 7), ("an", 2), ("a", 1)]

main = do
  putStrLn testPatricia