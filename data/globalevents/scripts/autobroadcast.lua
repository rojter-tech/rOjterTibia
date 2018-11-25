local messages = {
	"Por favor, reporte bugs e criaturas faltando/npcs/quests no forum de nosso servidor com imagens e descri��o.",
	"Siga as regras do servidor para evitar um banimento.",
	"Vire um guerreiro, transforme sua voca��o, aumente sua experiencia, obtenha items novos, e ajude o servidor permanecer online.",
	"Voc� tem a possibilidade de doar com cart�o de cr�dito, dep�sito, transferencias banc�rias ou atrav�s de boleto bancario no website RivalWar",
	"Visite o forum oficial diariamente para ter uma vis�o geral das �ltimas not�cias.",
	"A equipe do servidor n�o faz quests nem wars, ent�o se voc� quer um OT cheio de wars ajude-nos a divulgar para fazer diversas wars.",
	"N�o nos responsabilizamos por items/senhas perdidos, para evitar isso n�o de sua senha para ninguem e tenha sua Recovery Key anotada em seguran�a!!",
	"Convide seus amigos para jogar, fazer uma guild, ganhar dinheiro, encontrar itens raros e se tornar uma lenda no RivalWar.",
	"Aproveite o servidor por inteiro fazendo uma doa��o e adquirindo todas as vantagens que o mesmo lhe ofere�e",
	"Fique sabendo os horarios dos eventos acessando o site RivalWar. Os eventos podem acontecer tambem quando algum GameMaster inicia-lo!"
}

local i = 0
function onThink(interval, lastExecution)
local message = messages[(i % #messages) + 1]
    doBroadcastMessage("News: " .. message .. "", MESSAGE_STATUS_CONSOLE_ORANGE)
    i = i + 1
    return TRUE
end