
use sistema_gerenciamento_ecommerce;
-- Exemplo de Stored Procedure para realizar uma Compra
DELIMITER //

CREATE PROCEDURE RealizarCompra(
  IN p_cliente_id INT,
  IN p_produto_id INT,
  IN p_quantidade INT,
  IN p_data DATE
)
BEGIN
  DECLARE EXIT HANDLER FOR SQLEXCEPTION
  BEGIN
    ROLLBACK;
    RESIGNAL;
  END;

  START TRANSACTION;

  -- Inserir a compra
  INSERT INTO Compra (cliente_id, data)
  VALUES (p_cliente_id, p_data);

  -- Atualizar a quantidade disponível do produto no estoque
  UPDATE Estoque
  SET quantidade = quantidade + p_quantidade
  WHERE produto_id = p_produto_id;

  COMMIT;
END//

DELIMITER ;


-- Exemplo de Stored Procedure para realizar uma Venda
DELIMITER //

CREATE PROCEDURE RealizarVenda(
  IN p_cliente_id INT,
  IN p_produto_id INT,
  IN p_quantidade INT,
  IN p_data DATE,
  IN p_endereco VARCHAR(255)
)
BEGIN
  DECLARE EXIT HANDLER FOR SQLEXCEPTION
  BEGIN
    ROLLBACK;
    RESIGNAL;
  END;

  START TRANSACTION;

  -- Verificar se há estoque suficiente
  SELECT quantidade INTO @estoque_disponivel
  FROM Estoque
  WHERE produto_id = p_produto_id;

  IF @estoque_disponivel >= p_quantidade THEN
    -- Inserir a venda
    INSERT INTO Venda (cliente_id, data)
    VALUES (p_cliente_id, p_data);

    -- Atualizar a quantidade disponível do produto no estoque
    UPDATE Estoque
    SET quantidade = quantidade - p_quantidade
    WHERE produto_id = p_produto_id;

    -- Registrar a entrega
    INSERT INTO Entrega (venda_id, data, endereco)
    VALUES (LAST_INSERT_ID(), p_data, p_endereco);

    COMMIT;
  ELSE
    -- Falha na venda devido à falta de estoque
    ROLLBACK;
  END IF;
END//

DELIMITER ;


-- Exemplo de Stored Procedure para gerar uma Nota Fiscal
DELIMITER //

CREATE PROCEDURE GerarNotaFiscal(
  IN p_venda_id INT,
  IN p_pagamento VARCHAR(255)
)
BEGIN
  DECLARE EXIT HANDLER FOR SQLEXCEPTION
  BEGIN
    ROLLBACK;
    RESIGNAL;
  END;

  START TRANSACTION;

  -- Inserir a nota fiscal
  INSERT INTO NotaFiscal (venda_id, pagamento)
  VALUES (p_venda_id, p_pagamento);

  COMMIT;
END//

DELIMITER ;
