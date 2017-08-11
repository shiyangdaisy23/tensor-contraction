i = 1;
ti = zeros(11,1);
for n = 20:10:120
    T = tensor(rand(n,n,n));
     for j = 1:10
    tic
    X = tucker_als(T,[10,10,10],'tol',1.0e-15,'maxiters',200);
    ti(i) = ti(i)+toc;
     end
     ti(i) = ti(i)/10;
    i = i+1;
end