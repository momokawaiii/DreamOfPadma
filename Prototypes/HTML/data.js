(function (root) {
  'use strict';
  // This catalog belongs only to the browser rule prototype. No UE asset imports.
  const cards = {
    dawn: { id: 'dawn', family: 'A', attribute: 'F', name: '佛罗伦萨的曙光', icon: '☼', cost: 5, stability: 25, tendency: [2, 1], affinity: 10, tags: { 繁荣: 40, 新生: 20, 毁灭: 20, 知识: 20 }, page: 'mycard 16', dailyFlow: 2, coverage: '已接入：基本盘每日额外心流 +2、倾向。双壳穹顶与形意尚未接入。' },
    earth: { id: 'earth', family: 'A', attribute: 'F', name: '地痕', icon: '◈', cost: 4, stability: 12, tendency: [-1, 1], affinity: 10, tags: { 奥秘: 80, 禁忌: 20 }, page: 'mycard 14', dailyFlow: 1, dailyLimit: 3, coverage: '已接入：基本盘前三天额外心流 +1、倾向。阿波罗的魂灵、皮媞亚尚未接入。' },
    banquet: { id: 'banquet', family: 'A', attribute: 'F', name: '安提帕鸿门宴', icon: '♜', cost: 3, stability: 30, tendency: [4, 3], affinity: -10, tags: { 欲望: 60, 权力: 40 }, page: 'mycard 14–15', coverage: '已接入：每三天核心恢复 1、希律一诺（每时代一次 4 心流换 4 计算力或反向）。其他被动尚未接入。' },
    cherry: { id: 'cherry', family: 'B', attribute: 'D', name: '山樱一梦', icon: '❀', cost: 2, field: [1, 3], tags: { 繁荣: 80, 新生: 20 }, page: 'mycard 23', coverage: '已接入：场域、倾向累加、合成成功熵 −3、人物登场心流 +1。空门尚未接入。' },
    reed: { id: 'reed', family: 'B', attribute: 'D', name: '蓍', icon: '╱', cost: 1, field: [4, 8], tags: { 奥秘: 90, 知识: 10 }, page: 'mycard 20', coverage: '已接入：场域、倾向累加。禁用 A 被动／基本盘翻倍、登场与突变效果尚未接入。' },
    cleopatra: { id: 'cleopatra', family: 'B', attribute: 'D', name: '埃及艳后', icon: '♛', cost: 1, field: [-4, -5], tags: { 欲望: 30, 权力: 25 }, page: 'mycard 20', coverage: '已接入：场域、倾向累加。装饰、登场与突变效果尚未接入。' },
    vitruvian: { id: 'vitruvian', family: 'C', attribute: 'R', name: '维特鲁威人', icon: '✧', kind: 'person', target: [5, 6], atk: 0, def: 4, hp: 25, attackCost: 1, maxTargets: 1, tags: { 繁荣: 70, 新生: 30 }, page: 'mycard 29', coverage: '已接入：面板、普攻、食烟火（繁荣／欲望卡登场永久攻击 +1）、沙盘完美肉体。黄金分割尚未接入。' },
    dancer: { id: 'dancer', family: 'C', attribute: 'R', name: '希律的舞女', icon: '♢', kind: 'person', target: [16, 11], atk: 10, def: 2, hp: 45, attackCost: 6, maxTargets: 2, tags: { 欲望: 50, 权力: 50 }, page: 'mycard 26–27', coverage: '已接入：面板、普攻（最多两目标）。七重纱舞及其他专属技能尚未接入。' },
    door: { id: 'door', family: 'C', attribute: 'R', name: '犹格索托斯', icon: '▥', kind: 'building', target: [2, 10], atk: 2, def: 6, hp: 22, attackCost: 3, maxTargets: 1, tags: { 奥秘: 80, 禁忌: 10, 知识: 10 }, page: 'mycard 26', coverage: '已接入：面板、普攻、命中双方信仰 −1。其他专属技能尚未接入。' }
  };
  const skills = [
    { id: 'cut', name: '一刀两断', icon: '╱', page: 'mycard 1', target: 'enemy', flow: 1, calc: 0, costSource: 'HTML 临时费用', encounter: '一个非核心敌方单位：3 点真实伤害。', act: '锁定的非核心敌人：3 点真实伤害（HTML 效果配置）。' },
    { id: 'wind', name: '挟风鸣', icon: '≈', page: 'mycard 2', target: 'ally', flow: 1, calc: 0, costSource: 'HTML 临时费用', encounter: '一个友方单位获得 2 层归墟，可叠加。', act: 'ACT 出战角色获得 2 点归墟护盾。' },
    { id: 'mantra', name: '九字真言', icon: '◇', page: 'mycard 2', target: 'ally', flow: 1, calc: 0, costSource: 'PDF 费用', encounter: '一个友方单位获得 1 层抗衡，可叠加。', act: 'ACT 出战角色获得 1 次伤害格挡。' },
    { id: 'light', name: '得见光明', icon: '✦', page: 'mycard 3', target: 'ally', flow: 1, calc: 1, costSource: 'PDF 费用', encounter: '一个非核心友军恢复 5 生命；本次行动尚未行动则信仰 +1。', act: 'ACT 出战角色恢复 5 生命；本版 ACT 配置无回合条件。' },
    { id: 'rain', name: '鱼骸骤雨', icon: '⋮', page: 'mycard 1', target: 'all', flow: 2, calc: 0, costSource: 'HTML 临时费用', encounter: '敌我所有非核心单位：3 次物理伤害，每次 2 点。会误伤友军。', act: '场上敌我所有非核心角色：3 次物理伤害，每次 2 点。会误伤自己。' }
  ];
  const nodes = [
    { id: 'home', name: '归处', subtitle: '梦开始的地方', type: 'core', terrain: '城镇', x: 150, y: 320, icon: '⌂', owner: 'player', description: '己方核心与部署起点。点击查看卡牌；移动使用右侧按钮。' },
    { id: 'fire', name: '薪火台', subtitle: '沿途的温度', type: 'fire', terrain: '平原', x: 340, y: 210, icon: '♨', owner: 'neutral', description: '占领后每日提供额外心流。设施收益使用可修改的试玩数值。' },
    { id: 'story', name: '渡鸦驿站', subtitle: '一段未完成的故事', type: 'story', terrain: '林地', x: 340, y: 430, icon: '❧', owner: 'neutral', description: '与守望人交谈，获得一次资源援助并开启到熔炉的预设道路。好感加成尚待设计。' },
    { id: 'plain', name: '风蚀原', subtitle: '两条道路的交点', type: 'normal', terrain: '平原', x: 540, y: 320, icon: '〰', owner: 'neutral', description: '可以继续前往熔炉或关隘。视觉相邻不代表可以通行。' },
    { id: 'forge', name: '旧日熔炉', subtitle: '余烬尚存', type: 'forge', terrain: '丘陵', x: 735, y: 430, icon: '♜', owner: 'neutral', description: '占领后可使用一次熔炉补给，随后变为普通地块。' },
    { id: 'gate', name: '王庭关隘', subtitle: '公开的敌阵', type: 'gate', terrain: '山地', x: 735, y: 210, icon: '⚑', owner: 'ruler', enemies: 2, description: '全部敌情公开。点击只查看，移动命令才进入战斗准备。' },
    { id: 'boss', name: '王庭归处', subtitle: '抵达梦的另一端', type: 'rulerCore', terrain: '城镇', x: 930, y: 320, icon: '♛', owner: 'ruler', enemies: 2, description: '击败守卫后削减统治者核心生命。首次 HTML 的攻击量可在参数页修改。' }
  ];
  const edges = [ ['home','fire'], ['home','story'], ['fire','plain'], ['story','plain'], ['plain','forge'], ['plain','gate'], ['forge','boss'], ['gate','boss'], ['story','forge','story-road'] ];
  const profileFields = [
    ['seed','随机种子',731,1,2147483647], ['flowStart','开局心流',60,0,500], ['flowCap','心流上限',80,1,500], ['calcStart','开局计算力',40,0,500], ['calcCap','计算力上限',50,1,500],
    ['faith','开局信仰',60,0,100], ['entropy','开局熵',10,0,100], ['dailyFraction','每日基础恢复／上限',0.5,0.5,1], ['dailyDraw','每日 ABC 抽牌数',2,0,10], ['synthesisFlow','合成心流费用',2,0,50], ['synthesisCalc','合成计算力费用',3,0,50], ['temperature','Softmax 温度',0.22,0.01,2],
    ['moveA','移动 A 计算力',2,0,20], ['moveB','每张附着 B 额外计算力',0,0,20], ['movePerson','移动 C 人物计算力',4,0,20], ['moveBuilding','移动 C 建筑计算力',6,0,20], ['deployC','部署 C 心流费用',2,0,30],
    ['coreHP','双方初始核心生命',30,1,200], ['coreStrike','攻克王庭削减核心生命',30,1,200], ['warGoal','战局天平胜利边界',80,10,300], ['captureWar','普通地块占领权重',10,0,50], ['gateWar','关隘占领权重',40,0,100], ['fireIncome','薪火每日心流',4,0,30], ['forgeFlow','熔炉一次心流',12,0,100], ['storyCalc','剧情一次计算力',10,0,100],
    ['enemyHP','守卫生命',16,1,150], ['enemyAtk','守卫攻击',5,1,40], ['enemyDef','守卫防御',1,0,20], ['bossHP','王庭守卫生命',24,1,200], ['playerSpeed','Encounter 我方行动速度',12,1,40], ['enemySpeed','Encounter 守卫行动速度',8,1,40],
    ['actHP','ACT 占位旅人生命',50,1,200], ['actAtk','ACT 普攻基础攻击',8,1,60], ['actDef','ACT 防御',2,0,20], ['actSpeed','ACT 移速（像素／秒）',220,50,450], ['actRange','ACT 攻击距离（像素）',105,30,350], ['actCooldown','ACT 普攻间隔（秒）',0.55,0.1,3], ['enemyInterval','ACT 敌人攻击间隔（秒）',1.5,0.4,5]
  ];
  const defaults = Object.fromEntries(profileFields.map(f => [f[0], f[2]]));
  const actCharacters = [
    { id: 'traveller', name: '试玩旅人', palette: '#578877', trait: '轻步', description: '平原移动速度 +15%；山地停用此特性，角色仍可出战。仅用于 HTML 的可替换设计示例。', modelRef: '', skillTableRef: '' },
    { id: 'scout', name: '试玩巡行者', palette: '#927aa4', trait: '轻步', description: '使用与试玩旅人相同的临时战斗参数和轻步特性，仅用于验证独立 ACT 招募编队与角色展示。', modelRef: '', skillTableRef: '' }
  ];
  const weapons = [{ id: 'blade', name: '练习剑', damage: 1, range: 1, interval: 1 }, { id: 'spear', name: '练习长枪', damage: 0.85, range: 1.5, interval: 1.25 }];
  // Independent placeholder display catalogs. These selections never grant stats or ABC cards.
  const homeCharacters = [
    { id: 'keeper-a', name: '归处主角 A', palette: '#698174', hair: '#313c38', modelRef: '', skillTableRef: '' },
    { id: 'keeper-b', name: '归处主角 B', palette: '#947489', hair: '#dfd7ca', modelRef: '', skillTableRef: '' }
  ];
  const homeOutfits = [
    { id: 'travel', name: '远行装', description: '轻装披肩与旅行服 · HTML 展示占位' },
    { id: 'formal', name: '礼装', description: '长外套与垂饰 · HTML 展示占位' },
    { id: 'combat', name: '战装', description: '短披风与护具 · HTML 展示占位' }
  ];
  const homeWeapons = [
    { id: 'sword', name: '单手剑', poses: ['idle','ready','salute'] },
    { id: 'spear', name: '长枪', poses: ['idle','ready'] }
  ];
  const homePoses = [{ id: 'idle', name: '闲立' }, { id: 'ready', name: '备战' }, { id: 'salute', name: '执剑致意' }];
  const fpsCharacters = [{ id: 'fps-vanguard', name: 'FPS 先锋', palette: '#6e88a0' }, { id: 'fps-observer', name: 'FPS 观察员', palette: '#b79767' }];
  const fpsWeapons = [{ id: 'rifle', name: '练习步枪' }, { id: 'carbine', name: '练习卡宾枪' }];
  const dialogues = {
    story: { title: '守望人的路标 · 原创占位剧情', start: 'intro', lines: {
      intro: { speaker: '守望人', text: '你也听见了旧日熔炉的回响？渡鸦总是沿着那条小路回来。', next: 'offer' },
      offer: { speaker: '守望人', text: '我可以替你标出驿站通往熔炉的小路，再留下一份计算力补给。', choices: [
        { id: 'ask', label: '先问问那条小路', next: 'road' },
        { id: 'accept', label: '接受援助并解锁道路', complete: true },
        { id: 'leave', label: '暂时离开', cancel: true }
      ] },
      road: { speaker: '守望人', text: '这不是近在眼前就能走通的路。跟着我留下的路标，才能绕过断桥。准备好时，我再把补给交给你。', next: 'offer' }
    } },
    forge: { title: '余烬中的补给 · 原创占位剧情', start: 'intro', lines: {
      intro: { speaker: '你', text: '炉膛仍有温度。旧补给箱封在炉边，似乎只能打开一次。', next: 'offer' },
      offer: { speaker: '炉旁的手记', text: '给后来者：取走这份心流补给之后，让余烬安静地熄灭吧。', choices: [
        { id: 'inspect', label: '再读一页手记', next: 'note' },
        { id: 'accept', label: '领取补给，熔炉转为普通地块', complete: true },
        { id: 'leave', label: '先不领取', cancel: true }
      ] },
      note: { speaker: '炉旁的手记', text: '这里曾为远行者点灯。剩下的东西不多，却足够让下一段路明亮一些。', next: 'offer' }
    } }
  };
  const api = { version: 1, cards, skills, nodes, edges, profileFields, defaults, actCharacters, weapons, homeCharacters, homeOutfits, homeWeapons, homePoses, fpsCharacters, fpsWeapons, dialogues, eras: ['图腾','蒙昧','启蒙','死仇','摩登','朋克','次世代'], phases: ['黎明','晌午','黄昏'] };
  if (typeof module !== 'undefined' && module.exports) module.exports = api;
  else root.PadmaData = api;
})(typeof globalThis !== 'undefined' ? globalThis : this);
